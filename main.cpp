#include <QCoreApplication>
#include <lib_s.h>
#include <QDir>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <string>


//global
QFile Ans;
QString FolderPath = "", FilePath;

void file_folder(){


    if(!QDir(FolderPath+"Save").exists()){
        std::cout<<"Creating new folder!"<<std::endl;
        QDir().mkdir(FolderPath+"Save");
        std::cout<<"OK"<<std::endl;
    }
    else {
        std::cout<<"There is the folder."<<std::endl;
    }

    //file
    Ans.setFileName(FolderPath+"Save/"+"ans.txt");
    FilePath = FolderPath +"Save/" + "ans.txt";
    if(!Ans.exists()){
       std::cout<<"Creating file!"<<std::endl;
       Ans.open(QIODevice::WriteOnly);
       std::cout<<"OK"<<std::endl;
    }
    else{
         std::cout<<"There is the file."<<std::endl;
         Ans.open(QIODevice::WriteOnly);
    }

}


// get IP or path
std::string IP_P(){
    std::cout<<"Write IP address:"<<std::endl;
    std::string ip_add;
    std::cin>>ip_add;
    return ip_add;
}


void I_checker(QString &line){
    line.replace(QString("l"),QString("I"));
    line.replace(QString("|"),QString("I"));
    line.replace(QString("/"),QString("I"));
    line.replace(QString("J"),QString("I"));
    line.replace(QString("!"),QString("I"));
    line.replace(QString("t"),QString("I"));
}

void detectANDocr()
{
    cv::CascadeClassifier car;
    std::string cascade_for_car= "car_cascade.xml";
    car.load(cascade_for_car);

    if (car.empty()){
        std::cout<<"Some problems with cascade!"<<std::endl;
    }
    else {
        std::cout<<"Cascade load successfully!"<<std::endl;
    }

    char *old_type = strdup(setlocale(LC_ALL,NULL));
    setlocale(LC_ALL,"C");
    tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();

    QString ocr_txt_res;
    cv::VideoCapture cap(IP_P());
    if(!cap.isOpened()){
        std::cout<<"Video doesn`t open!"<<std::endl;
    }
    /*int vid_w = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int vid_h = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    cap.get(cv::CAP_PROP_FOURCC)      to do:  ad array char to make izi set fourcc (if i'll write video)
    int fps_v = 30;
    char in1 = 'M';
    char in2 = 'J';
    char in3 = 'P';
    char in4 = 'G';
    cv::VideoWriter vidos("res_vid6.avi",cv::VideoWriter::fourcc(in1, in2, in3, in4), fps_v , cv::Size(vid_w,vid_h),true);*/

    QTextStream out(&Ans);
    out<<"start";
    cv::Mat frame;
    cv::Mat gray_frame;
    cv::Mat detect_rec;
    int num=0;
    std::vector<cv::Rect> plate;
    std::string rec_name = "ps_pl" ;

    int test =0;

    while(true){
        cap.read(frame);
        if (frame.empty()){
            std::cout<<"Frame is empty"<<std::endl;
            break;
        }

        cv::cvtColor(frame,gray_frame, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray_frame,gray_frame);
        car.detectMultiScale(gray_frame, plate);

        for ( size_t i = 0; i < plate.size(); i++ ){
            //cv::rectangle(frame, plate[i],  Scalar(255,0,0), 2, LINE_8);
            detect_rec = frame( plate[i] );
            cv::cvtColor(detect_rec,detect_rec, cv::COLOR_BGR2GRAY);
            cv::threshold(detect_rec,detect_rec,200,255,cv::THRESH_BINARY+cv::THRESH_OTSU);
            cv::blur(detect_rec,detect_rec,cv::Size(3,3));
            //cv::erode(roi,roi,Mat(),Point(-1,-1));
            //cv::dilate(roi,roi,Mat(),Point(-1,-1));
            //cv::imwrite("Save/ps_pl"+std::to_string(test)+".jpg",detect_rec);
            //test++;

            ocr->Init(NULL,"eng");
            ocr->SetImage((uchar*)detect_rec.data, detect_rec.size().width, detect_rec.size().height, detect_rec.channels(), detect_rec.step1());
            ocr->SetSourceResolution(200);          // should change, it depends of camera
            ocr_txt_res = ocr->GetUTF8Text();
            ocr_txt_res.remove(QChar(' '));
            ocr_txt_res.remove(QChar('\n'));


            if( ocr_txt_res.size()>=6 && ocr_txt_res.size()<=10){
                I_checker(ocr_txt_res);
                std::cout<<"=";
                if(num%10==1){
                    std::cout<<std::endl;
                }
                rec_name += std::to_string(num)+ ".jpg";
                cv::imwrite("Save/ps_pl"+rec_name,detect_rec);
                num++;
               // QTextStream out(&Ans);
                rec_name+=" : ";
                out<< QString::fromStdString(rec_name)+ocr_txt_res + "\n";
            }

            ocr_txt_res.clear();
            rec_name.clear();
        }

    }

}







int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    std::cout << "OpenCV version : " << CV_VERSION << std::endl;
    file_folder();
    detectANDocr();
    Ans.close();
    return a.exec();
}
