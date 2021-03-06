#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <QImage>
#include <QApplication>
#include <QLabel>
#include <QSettings>
#include <Windows.h>
#include <winreg.h>
#include <ConsoleApi3.h>
class Settings{
public:
    bool Init();

    inline bool WhetherBlur()const;
    inline bool WhetherTrace()const;
    inline bool WhetherMirror()const;
    inline bool WhetherDebug()const;
    double GetWindowOpacity()const;
    inline int GetWindowWidth()const;
    inline int GetWindowHeight()const;
    inline int GetDelay()const;
    int GetWindowTop()const;
    int GetWindowLeft()const;
    QRect GetWindowRect()const;

    bool TransparentForMouse()const;
private:
    bool ReadDword(const wchar_t* ValueName,DWORD &Value)const;
    HKEY hKey;
    DWORD Do_Bluring=0;
    DWORD Opacity=0;
    DWORD TraceFace=0;
    DWORD WindowWidth=0;
    DWORD WindowHeight=0;
    DWORD AllowMouseThrough=0;
    DWORD Position_Left;
    DWORD Position_Top;
    DWORD ShowInMirror;
    DWORD Delay;
    DWORD DebugMode;
};
inline bool Settings::WhetherBlur()const{return Do_Bluring;}
inline bool Settings::WhetherTrace()const{return TraceFace;}
inline bool Settings::WhetherMirror()const{return ShowInMirror;}
inline bool Settings::WhetherDebug()const{return DebugMode;}
double Settings::GetWindowOpacity()const{return static_cast<double>(Opacity)/100;}
inline int Settings::GetWindowWidth()const{return WindowWidth;}
inline int Settings::GetWindowHeight()const{return WindowHeight;}
inline int Settings::GetDelay()const{if(Delay>=1 && Delay<1000)return Delay;if(Delay==0)return 1;if(Delay>999)return 1000;};
int Settings::GetWindowTop()const{return Position_Top;}
int Settings::GetWindowLeft()const{return Position_Left;}
QRect Settings::GetWindowRect()const{return QRect(GetWindowLeft(),GetWindowTop(),GetWindowWidth(),GetWindowHeight());}
bool Settings::TransparentForMouse()const{return AllowMouseThrough;}

bool Settings::Init(){
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE,L"Software\\6ziv\\CameraOnScreen",0,KEY_QUERY_VALUE,&hKey)!=ERROR_SUCCESS)
        return false;
    if(!ReadDword(L"Bluring",Do_Bluring))return false;
    if(!ReadDword(L"Tracing",TraceFace))return false;
    if(!ReadDword(L"WindowWidth",WindowWidth))return false;
    if(!ReadDword(L"WindowHeight",WindowHeight))return false;
    if(!ReadDword(L"WindowLeft",Position_Left))return false;
    if(!ReadDword(L"WindowTop",Position_Top))return false;
    if(!ReadDword(L"WindowOpacity",Opacity))return false;
        if(Opacity>100)Opacity=100;
    if(!ReadDword(L"TransparentForMouse",AllowMouseThrough))return false;
    if(!ReadDword(L"ShowInMirror",ShowInMirror))return false;
    if(!ReadDword(L"Debug",DebugMode))return false;
    if(!ReadDword(L"Delay",Delay))return false;
    RegCloseKey(hKey);
    return true;
}
bool Settings::ReadDword(const wchar_t *ValueName, DWORD &Value)const{
    DWORD RegType;
    DWORD cbData=sizeof(DWORD);
    if(ERROR_SUCCESS!=RegQueryValueExW(hKey,ValueName,NULL,&RegType,reinterpret_cast<LPBYTE>(&Value),&cbData))return false;
    if(RegType!=REG_DWORD || cbData!=sizeof(DWORD))return false;
    return true;
}

class ImageFactory{
public:
    void Init(const Settings *settings);
    void Unload();
    void GrubImage();
    void ProcessImage();
    cv::Mat &Output();
private:
    void FaceRec();
    void Blur_Func();
    void Trace_Func();
    bool hasface;
    bool used=false;
    const Settings *Configuration;
    cv::VideoCapture Recorder;
    cv::CascadeClassifier FaceIdentifier;
    std::vector<cv::Rect> faces;
    cv::Rect face;
    cv::Rect TargetRect;
    cv::Rect OldRect;
    cv::Mat Frame;
    cv::Mat GrayFrame;
    cv::Mat FaceFrame;
    cv::Mat OutputFrame;
    int DifferCount;

};
void ImageFactory::Init(const Settings *settings){
    Configuration=settings;
    Recorder.open(0);
    FaceIdentifier.load("./haarcascade_frontalface_alt.xml");
    DifferCount=0;
}
void ImageFactory::Unload(){
    Recorder.release();
}
void ImageFactory::GrubImage(){
    Recorder.grab();
    Recorder.retrieve(Frame);
    int OutputLeft;
    int OutputTop;
    int OutputWidth;
    int OutputHeight;
    OutputLeft=OutputTop=0;
    OutputWidth=Frame.cols;
    OutputHeight=Frame.rows;
    if(OutputWidth*Configuration->GetWindowHeight()>OutputHeight*Configuration->GetWindowWidth()){
        OutputWidth=OutputHeight*Configuration->GetWindowWidth()/Configuration->GetWindowHeight();
        OutputLeft=(Frame.cols-OutputHeight*Configuration->GetWindowWidth()/Configuration->GetWindowHeight())/2;
    }else if(OutputWidth*Configuration->GetWindowHeight()<OutputHeight*Configuration->GetWindowWidth()){
        OutputHeight=OutputWidth*Configuration->GetWindowHeight()/Configuration->GetWindowWidth();
        OutputTop=(Frame.rows-OutputWidth*Configuration->GetWindowHeight()/Configuration->GetWindowWidth())/2;
    }
    TargetRect=cv::Rect(OutputLeft,OutputTop,OutputWidth,OutputHeight);

}
void ImageFactory::ProcessImage(){
    FaceRec();
    if(Configuration->WhetherBlur())Blur_Func();
    if(Configuration->WhetherTrace())Trace_Func();
}
void ImageFactory::FaceRec(){
    cv::cvtColor(Frame,GrayFrame,cv::COLOR_BGR2GRAY);
    cv::equalizeHist(GrayFrame,GrayFrame);
    faces.clear();
    FaceIdentifier.detectMultiScale(GrayFrame,faces,1.1,2,cv::CASCADE_DO_CANNY_PRUNING,cv::Size(50,50));
    if(faces.size()==0){hasface=false;return;}
    hasface=true;
    face=*std::max_element(faces.begin(),faces.end(),[](cv::Rect x,cv::Rect y)->bool{return x.size().area()<y.size().area();});
    faces.clear();
    FaceFrame=Frame(face);
}
void ImageFactory::Blur_Func(){
    if(!hasface){
        return;
    }
    cv::Mat tmp;
    int value1 = 3, value2 = 1;
    int dx = value1 * 5;
    double fc = value1*12.5;
    int p = 50;
    cv::Mat temp1, temp2, temp3, temp4;
    bilateralFilter(FaceFrame, temp1, dx, fc, fc);
    temp2 = (temp1 - FaceFrame + 128);
    GaussianBlur(temp2, temp3, cv::Size(2 * value2 - 1, 2 * value2 - 1), 0, 0);
    temp4 = FaceFrame + 2 * temp3 - 255;
    tmp = (FaceFrame*(100 - p) + temp4*p) / 100;
    tmp.copyTo(FaceFrame);
}
void ImageFactory::Trace_Func(){
    if(!hasface){
        if(used){
            TargetRect=OldRect;
        }
        return;
    }
    used=true;
    int tmpWidth=face.width*2;
    int tmpHeight=face.height*2;
    if(tmpWidth*Configuration->GetWindowHeight()>tmpHeight*Configuration->GetWindowWidth()){
        tmpHeight=tmpWidth*Configuration->GetWindowHeight()/Configuration->GetWindowWidth();
    }else{
        tmpWidth=tmpHeight*Configuration->GetWindowWidth()/Configuration->GetWindowHeight();
    }
    if(tmpWidth>Frame.cols)tmpWidth=Frame.cols;
    if(tmpHeight>Frame.rows)tmpHeight=Frame.rows;
    if(tmpWidth*Configuration->GetWindowHeight()>tmpHeight*Configuration->GetWindowWidth()){
        tmpWidth=tmpHeight*Configuration->GetWindowWidth()/Configuration->GetWindowHeight();
    }else{
        tmpHeight=tmpWidth*Configuration->GetWindowHeight()/Configuration->GetWindowWidth();
    }
    int tmpLeft,tmpTop;
    tmpLeft=face.x+face.width/2-tmpWidth/2;
    tmpTop=face.y+face.height/2-tmpHeight/2;
    if(tmpLeft<0)tmpLeft=0;
    if(tmpTop<0)tmpTop=0;
    if(tmpLeft+tmpWidth>Frame.cols)tmpLeft=Frame.cols-tmpWidth;
    if(tmpTop+tmpHeight>Frame.rows)tmpTop=Frame.rows-tmpHeight;
    if(std::abs(OldRect.x-tmpLeft)<40 &&std::abs(OldRect.y-tmpTop)<40 && std::abs(OldRect.width-tmpWidth)<20 && std::abs(OldRect.height-tmpHeight)<20)
        TargetRect=OldRect;
    else{
        TargetRect=cv::Rect(tmpLeft,tmpTop,tmpWidth,tmpHeight);
        OldRect=TargetRect;
    }
}
cv::Mat &ImageFactory::Output(){
    cv::cvtColor(Frame(TargetRect),OutputFrame,cv::COLOR_BGR2RGB);
    return this->OutputFrame;
}

int main( int argc, char** argv )
{

    Settings settings;
    if(!settings.Init()){
        MessageBoxW(0,L"无法加载配置内容。请先配置!",L"错误",MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
        return 0;
    }
    if(settings.WhetherDebug())
        AllocConsole();

    QApplication a(argc,argv);
    QLabel label;
    if(settings.TransparentForMouse()){
        label.setAttribute(Qt::WA_TransparentForMouseEvents,true);
    }
    label.setWindowFlags(Qt::Tool|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    label.setGeometry(settings.GetWindowRect());
    label.show();
    label.setWindowOpacity(settings.GetWindowOpacity());

    ImageFactory imageFactory;
    imageFactory.Init(&settings);
    HANDLE Stdout=GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD ret;

    DWORD Tick;
    DWORD OldTick=0;
    while(1)
    {
        imageFactory.GrubImage();
        imageFactory.ProcessImage();
        cv::Mat OutputMat=imageFactory.Output();

        Tick=GetTickCount();
        DWORD TimeDiff=Tick-OldTick;
        OldTick=Tick;
        QString TimeTag=(QString("[")+QString::number(Tick)+QString("]"));
        WriteConsoleA(Stdout,TimeTag.toStdString().c_str(),TimeTag.length(),&ret,0);
        if(settings.WhetherBlur())WriteConsoleA(Stdout,"Bluring -- on , ",16,&ret,0);
        else WriteConsoleA(Stdout,"Bluring -- off, ",16,&ret,0);
        if(settings.WhetherTrace())WriteConsoleA(Stdout,"Face Tracing -- on , ",22,&ret,0);
        else WriteConsoleA(Stdout,"Face Tracing -- off, ",22,&ret,0);

        if(TimeDiff==0)TimeDiff=4274767295;

        QString fps=QString("fps:")+QString::number(static_cast<double>(1000)/TimeDiff)+QString("frames/s\n");
        WriteConsoleA(Stdout,fps.toStdString().c_str(),fps.length(),&ret,0);
        QImage img=QImage(OutputMat.data,OutputMat.cols,OutputMat.rows,OutputMat.step,QImage::Format::Format_RGB888).copy().scaled(settings.GetWindowWidth(),settings.GetWindowHeight());
        if(settings.WhetherMirror()){img=img.mirrored(true,false);}
        QPixmap pix=QPixmap::fromImage(img);
        label.setPixmap(pix);
        a.processEvents();
        Sleep(settings.GetDelay());
    }

}
