#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
extern "C"{
#include "libavformat/avformat.h"
}
static bool openFile(const char *url);
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    openFile("v1080.mp4");

}
static bool openFile(const char *url){

    av_register_all();
    avformat_network_init();

    AVDictionary *opts = nullptr;
    AVFormatContext *ic = NULL;
    int videoStream = 0;
    int audioStream = 1;
    int width  = 0;
    int height = 1;
    int sampleRate = 0;
    int channels   = 1;
    av_dict_set(&opts,"rtsp_transport" ,"tcp",0);
    av_dict_set(&opts,"max_delay" ,"500",0);
    int re = avformat_open_input(
                &ic,
                url,
                0,//auto decoder
                &opts //import opts
                );
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        qDebug() << "open " << url << " failed! :" << buf << endl;
        return false;
    }

    re = avformat_find_stream_info(ic,0);

   // this->totalMs = ic->duration/(AV_TIME_BASE/1000);
    //打印
    av_dump_format(ic, 0, url, 0);

    //获取视频流
    videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVStream *as = ic->streams[videoStream];
    width = as->codecpar->width;
    height = as->codecpar->height;

    qDebug() << "=======================================================" << endl;
    qDebug() << videoStream << "视频信息" << endl;
    qDebug() << "codec_id = " << as->codecpar->codec_id << endl;
    qDebug() << "format = " << as->codecpar->format << endl;
    qDebug() << "width=" << as->codecpar->width << endl;
    qDebug() << "height=" << as->codecpar->height << endl;
    //帧率 fps 分数转换
    //qDebug() << "video fps = " << av_q2d(as->avg_frame_rate) << endl;

    qDebug() << "=======================================================" << endl;
    qDebug() << audioStream << "音频信息" << endl;
    //获取音频流
    audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if(audioStream<0){
        return false;
    }
    as = ic->streams[audioStream];
    sampleRate = as->codecpar->sample_rate;
    channels  = as->codecpar->channels;

    qDebug() << "codec_id = " << as->codecpar->codec_id << endl;
    qDebug() << "format = " << as->codecpar->format << endl;
    qDebug() << "sample_rate = " << as->codecpar->sample_rate << endl;
    //AVSampleFormat;
    qDebug() << "channels = " << as->codecpar->channels << endl;
    //一帧数据？？ 单通道样本数
    qDebug() << "frame_size = " << as->codecpar->frame_size << endl;
    //1024 * 2 * 2 = 4096  fps = sample_rate/frame_size
    return true;
}
MainWindow::~MainWindow()
{
    delete ui;
}

