#include <iostream>
#include "ffmpegsdl.h"
#include <stdio.h>  
#include <winsock2.h>  
#include <windows.h>
#include <cstring>
#include <iostream>
#include <boost/thread/thread.hpp>
#include <atlbase.h>  
#include<ctime>
//#include <sys/time.h>
//#include <thread>
//#include <atlbase.h>  
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include "libavcodec/avcodec.h"
}
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib, "ws2_32.lib")   
#define ASSERT(X) if(!(X)){printf("####[%s:%d]assert failed:%s\n", __FUNCTION__, __LINE__, #X);}
#define _READ_BUF_SIZE (4<<10)
//广播包
BYTE buffer[] = { 0xf1, 0x30, 0x00, 0x00 };
//请求视频
BYTE buffer2[] = {
	0xf1, 0xd0, 0x00, 0xbe, 0xd1, 0x00, 0x00, 0x00,
	0x01, 0x0a, 0x00, 0x00, 0x61, 0x00, 0x00, 0x00,
	0x47, 0x45, 0x54, 0x20, 0x2f, 0x6c, 0x69, 0x76,
	0x65, 0x73, 0x74, 0x72, 0x65, 0x61, 0x6d, 0x2e,
	0x63, 0x67, 0x69, 0x3f, 0x73, 0x74, 0x72, 0x65,
	0x61, 0x6d, 0x69, 0x64, 0x3d, 0x31, 0x30, 0x26,
	0x73, 0x75, 0x62, 0x73, 0x74, 0x72, 0x65, 0x61,
	0x6d, 0x3d, 0x30, 0x26, 0x6c, 0x6f, 0x67, 0x69,
	0x6e, 0x75, 0x73, 0x65, 0x3d, 0x61, 0x64, 0x6d,
	0x69, 0x6e, 0x26, 0x6c, 0x6f, 0x67, 0x69, 0x6e,
	0x70, 0x61, 0x73, 0x3d, 0x38, 0x38, 0x38, 0x38,
	0x38, 0x38, 0x26, 0x75, 0x73, 0x65, 0x72, 0x3d,
	0x61, 0x64, 0x6d, 0x69, 0x6e, 0x26, 0x70, 0x77,
	0x64, 0x3d, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38,
	0x26, 0x01, 0x0a, 0x00, 0x00, 0x49, 0x00, 0x00,
	0x00, 0x47, 0x45, 0x54, 0x20, 0x2f, 0x63, 0x68,
	0x65, 0x63, 0x6b, 0x5f, 0x75, 0x73, 0x65, 0x72,
	0x2e, 0x63, 0x67, 0x69, 0x3f, 0x6c, 0x6f, 0x67,
	0x69, 0x6e, 0x75, 0x73, 0x65, 0x3d, 0x61, 0x64,
	0x6d, 0x69, 0x6e, 0x26, 0x6c, 0x6f, 0x67, 0x69,
	0x6e, 0x70, 0x61, 0x73, 0x3d, 0x38, 0x38, 0x38,
	0x38, 0x38, 0x38, 0x26, 0x75, 0x73, 0x65, 0x72,
	0x3d, 0x61, 0x64, 0x6d, 0x69, 0x6e, 0x26, 0x70,
	0x77, 0x64, 0x3d, 0x38, 0x38, 0x38, 0x38, 0x38,
	0x38, 0x26 };
//
BYTE buffer3[] = {
	0xf1, 0x41, 0x00, 0x14, 0x56, 0x53, 0x54, 0x42,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x18, 0x72,
	0x52, 0x4d, 0x53, 0x52, 0x50, 0x00, 0x00, 0x00 };
//左转
BYTE buffer4[] = {
	0xf1,0xd0,0x00,0x6e,0xd1,0x00,0x00,0x01,0x01,
	0x0a,0x00,0x00,0x62,0x00,0x00,0x00,0x47,0x45,
	0x54,0x20,0x2f,0x64,0x65,0x63,0x6f,0x64,0x65,
	0x72,0x5f,0x63,0x6f,0x6e,0x74,0x72,0x6f,0x6c,
	0x2e,0x63,0x67,0x69,0x3f,0x63,0x6f,0x6d,0x6d,
	0x61,0x6e,0x64,0x3d,0x37,0x26,0x6f,0x6e,0x65,
	0x73,0x74,0x65,0x70,0x3d,0x30,0x26,0x6c,0x6f,
	0x67,0x69,0x6e,0x75,0x73,0x65,0x3d,0x61,0x64,
	0x6d,0x69,0x6e,0x26,0x6c,0x6f,0x67,0x69,0x6e,
	0x70,0x61,0x73,0x3d,0x35,0x32,0x31,0x35,0x32,
	0x31,0x26,0x75,0x73,0x65,0x72,0x3d,0x61,0x64,
	0x6d,0x69,0x6e,0x26,0x70,0x77,0x64,0x3d,0x35,
	0x32,0x31,0x35,0x32,0x31,0x26 };
BYTE buffer41[] = {
	0xf1,0xd0,0x00,0x6e,0xd1,0x00,0x00,0x04,0x01,
	0x0a,0x00,0x00,0x62,0x00,0x00,0x00,0x47,0x45,
	0x54,0x20,0x2f,0x64,0x65,0x63,0x6f,0x64,0x65,
	0x72,0x5f,0x63,0x6f,0x6e,0x74,0x72,0x6f,0x6c,
	0x2e,0x63,0x67,0x69,0x3f,0x63,0x6f,0x6d,0x6d,
	0x61,0x6e,0x64,0x3d,0x36,0x26,0x6f,0x6e,0x65,
	0x73,0x74,0x65,0x70,0x3d,0x30,0x26,0x6c,0x6f,
	0x67,0x69,0x6e,0x75,0x73,0x65,0x3d,0x61,0x64,
	0x6d,0x69,0x6e,0x26,0x6c,0x6f,0x67,0x69,0x6e,
	0x70,0x61,0x73,0x3d,0x35,0x32,0x31,0x35,0x32,
	0x31,0x26,0x75,0x73,0x65,0x72,0x3d,0x61,0x64,
	0x6d,0x69,0x6e,0x26,0x70,0x77,0x64,0x3d,0x35,
	0x32,0x31,0x35,0x32,0x31,0x26 };

//拍照
BYTE buffer7[] = {
	0xf1,0xd0,0x00,0x59,0xd1,0x00,0x00,0x01,0x01,
	0x0a,0x00,0x00,0x49,0x00,0x00,0x00,0x47,0x45,
	0x54,0x20,0x2f,0x73,0x6e,0x61,0x70,0x73,0x68,
	0x6f,0x74,0x2e,0x63,0x67,0x69,0x3f,0x6c,0x6f,
	0x67,0x69,0x6e,0x75,0x73,0x65,0x3d,0x61,0x64,
	0x6d,0x69,0x6e,0x26,0x6c,0x6f,0x67,0x69,0x6e,
	0x70,0x61,0x73,0x3d,0x35,0x32,0x31,0x35,0x32,
	0x31,0x26,0x75,0x73,0x65,0x72,0x3d,0x61,0x64,
	0x6d,0x69,0x6e,0x26,0x70,0x77,0x64,0x3d,0x35,
	0x32,0x31,0x35,0x32,0x31,0x26,0x72,0x65,0x73,
	0x3d,0x31,0x26 };
BYTE buffer5[] = {
	0xf1, 0xd1, 0x00, 0x06,0xd1,0x00,0x00,0x01,0x00,0x50 };
BYTE buffer8[] = {
	0xf1, 0xe1, 0x00, 0x00 };
BYTE buffer9[] = {
	0xf1, 0xe0, 0x00, 0x00 };
//get_status.cgi
BYTE buffer6[] = {
	0xf1,0xd0,0x00,0xad,0xd1,0x00,0x00,0x01,0x01,
	0x0a,0x00,0x00,0x49,0x00,0x00,0x00,0x47,0x45,
	0x54,0x20,0x2f,0x67,0x65,0x74,0x5f,0x73,0x74,
	0x61,0x74,0x75,0x73,0x2e,0x63,0x67,0x69,0x3f,
	0x6c,0x6f,0x67,0x69,0x6e,0x75,0x73,0x65,0x3d,
	0x61,0x64,0x6d,0x69,0x6e,0x26,0x6c,0x6f,0x67,
	0x69,0x6e,0x70,0x61,0x73,0x3d,0x38,0x38,0x38,
	0x38,0x38,0x38,0x26,0x75,0x73,0x65,0x72,0x3d,
	0x61,0x64,0x6d,0x69,0x6e,0x26,0x70,0x77,0x64,
	0x3d,0x38,0x38,0x38,0x38,0x38,0x38,0x26,0x01,
	0x0a,0x00,0x00,0x50,0x00,0x00,0x00,0x47,0x45,
	0x54,0x20,0x2f,0x67,0x65,0x74,0x5f,0x66,0x61,
	0x63,0x74,0x6f,0x72,0x79,0x5f,0x70,0x61,0x72,
	0x61,0x6d,0x2e,0x63,0x67,0x69,0x3f,0x6c,0x6f,
	0x67,0x69,0x6e,0x75,0x73,0x65,0x3d,0x61,0x64,
	0x6d,0x69,0x6e,0x26,0x6c,0x6f,0x67,0x69,0x6e,
	0x70,0x61,0x73,0x3d,0x38,0x38,0x38,0x38,0x38,
	0x38,0x26,0x75,0x73,0x65,0x72,0x3d,0x61,0x64,
	0x6d,0x69,0x6e,0x26,0x70,0x77,0x64,0x3d,0x38,
	0x38,0x38,0x38,0x38,0x38,0x26 };
//521521密码的请求视频
BYTE buffer1[] = {
	0xf1, 0xd0, 0x00, 0xbe, 0xd1, 0x00, 0x00, 0x00,
	0x01, 0x0a, 0x00, 0x00, 0x61, 0x00, 0x00, 0x00,
	0x47, 0x45, 0x54, 0x20, 0x2f, 0x6c, 0x69, 0x76,
	0x65, 0x73, 0x74, 0x72, 0x65, 0x61, 0x6d, 0x2e,
	0x63, 0x67, 0x69, 0x3f, 0x73, 0x74, 0x72, 0x65,
	0x61, 0x6d, 0x69, 0x64, 0x3d, 0x31, 0x30, 0x26,
	0x73, 0x75, 0x62, 0x73, 0x74, 0x72, 0x65, 0x61,
	0x6d, 0x3d, 0x30, 0x26, 0x6c, 0x6f, 0x67, 0x69,
	0x6e, 0x75, 0x73, 0x65, 0x3d, 0x61, 0x64, 0x6d,
	0x69, 0x6e, 0x26, 0x6c, 0x6f, 0x67, 0x69, 0x6e,
	0x70, 0x61, 0x73, 0x3d, 0x35, 0x32, 0x31, 0x35,
	0x32, 0x31, 0x26, 0x75, 0x73, 0x65, 0x72, 0x3d,
	0x61, 0x64, 0x6d, 0x69, 0x6e, 0x26, 0x70, 0x77,
	0x64, 0x3d, 0x35, 0x32, 0x31, 0x35, 0x32, 0x31,
	0x26, 0x01, 0x0a, 0x00, 0x00, 0x49, 0x00, 0x00,
	0x00, 0x47, 0x45, 0x54, 0x20, 0x2f, 0x63, 0x68,
	0x65, 0x63, 0x6b, 0x5f, 0x75, 0x73, 0x65, 0x72,
	0x2e, 0x63, 0x67, 0x69, 0x3f, 0x6c, 0x6f, 0x67,
	0x69, 0x6e, 0x75, 0x73, 0x65, 0x3d, 0x61, 0x64,
	0x6d, 0x69, 0x6e, 0x26, 0x6c, 0x6f, 0x67, 0x69,
	0x6e, 0x70, 0x61, 0x73, 0x3d, 0x35, 0x32, 0x31,
	0x35, 0x32, 0x31, 0x26, 0x75, 0x73, 0x65, 0x72,
	0x3d, 0x61, 0x64, 0x6d, 0x69, 0x6e, 0x26, 0x70,
	0x77, 0x64, 0x3d, 0x35, 0x32, 0x31, 0x35, 0x32,
	0x31, 0x26
};

#include <string.h>  

using namespace std;
uint8_t readb[9][30001] = { 0 };
int n = 0;
int getdiff(char ch);
int t = 0;
const int pixel_w = 1280, pixel_h = 720;
//Output YUV420P 
#define OUTPUT_YUV420P 0
FILE *fp_open = NULL;
int filescount = 0;
char filepath2[] = "v260.264";
char filetxt[] = "v260.txt";
int filecount = 0;
char filepath[] = "v261.264";
int bytecount = 0;
int first_time = 1;
int r = 0;
//回调函数
int j = 0;
int i = 0;
int nul = 0;
int read_buffer(void *opaque, uint8_t *buf, int buf_size) {
//pool:
//	if (r < t) 
//	{ 
//		r++; 
//	}
//	else goto pool;
	memcpy((buf), (readb[filecount % 9]), 10000 * sizeof(uint8_t));
	filecount += 1;
	return 10000;
}
void recvw() {
	//FILE *FSPOINTER;
	//filetxt[3] =( '0'+(filescount % 9));
	//fopen_s(&FSPOINTER, filepath2, "ab+");
	int lent = 0;
	int i = 8;
	char recvbuf[1200];

	WORD socketVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		return;
	}
	SOCKET sclient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	SOCKADDR_IN addrSrv;
	// addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY); 
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(32108);
	addrSrv.sin_addr.S_un.S_addr = inet_addr("192.168.1.255");
	//INADDR_BROADCAST inet_addr("192.168.1.255");  
	int len = sizeof(addrSrv);

	//char * sendData = "来自客户端的数据包.\n";  
	//send( sclient,(LPCSTR)buffer, sizeof(buffer), 0); 
	//发送寻找局域网内摄像头的buffer数据包
	printf("发送广播包\n");
	sendto(sclient, (LPCSTR)buffer, sizeof(buffer), 0, (sockaddr *)&addrSrv, len);
	SOCKADDR_IN addrClient;
	//接收客户端回应

	recvfrom(sclient, recvbuf, sizeof(recvbuf), 0, (SOCKADDR *)&addrClient, &len);

	printf("接收到客户端回应\n");

	for (int q = 0; q<10; q++) { printf("%x\n", (unsigned char)recvbuf[q]); }
	closesocket(sclient);
	sclient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(32108);
	bind(sclient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	len = sizeof(SOCKADDR);
	//recvfrom(sclient, recvbuf, sizeof(recvbuf), 0, (SOCKADDR *)&addrClient, &len);



	Sleep(200);
	//发送buffer3（UID）信息，类似于登录
	sendto(sclient, (LPCSTR)buffer3, sizeof(buffer3), 0, (sockaddr *)&addrClient, len); //回应
	Sleep(2000);

	sendto(sclient, (LPCSTR)buffer1, sizeof(buffer1), 0, (sockaddr *)&addrClient, len); //请求视频

	BYTE bufferack[] = {
		0xf1, 0xd1, 0x00, 0x08,0xd1,0x01,0x00,
		0x02,
		0x00,0x00,0x00,0x01,
	};
	BYTE bufferackff[] = {
		0xf1, 0xd1, 0x00, 0x06,0xd1,0x01,0x00,
		0x01,
		0x00,0xff,
	};
	t = 0;
	//loop:
	while (1) {

		if (n == 1) {
			break;
		}
		//接受
		recvfrom(sclient, recvbuf, sizeof(recvbuf), 0, (SOCKADDR *)&addrClient, &len);
		//判断是否是视频帧，获取帧号

		if ((unsigned char)recvbuf[4] == 0xd1 && (unsigned char)recvbuf[5] == 0x01) {

			if ((unsigned char)recvbuf[8] == 0x55 && (unsigned char)recvbuf[9] == 0xaa) {
				//i是读取数据的起始点
				i = 41;


			}
			else {
				i = 8;
				//fopen_s(&FSPOINTER, "C:/Users/36970/Documents/software/ipcamera/v264.txt", "a+");
			}


			if ((unsigned char)recvbuf[7] % 2 == 0x01 && (unsigned char)recvbuf[7] != 0xFF)
			{
				int p = 11;
				unsigned char var = recvbuf[7];
				for (char q = 0x00; q < 0x02; q++)
				{
					bufferack[p] = var;
					var = var - 0x01;
					p = p - 2;
				}
				bufferack[8] = recvbuf[6];
				bufferack[10] = recvbuf[6];
				sendto(sclient, (LPCSTR)bufferack, sizeof(bufferack), 0, (sockaddr *)&addrClient, len); //回应
																										//q++;
			}
			else if ((unsigned char)recvbuf[7] == 0xFF) {
				bufferackff[8] = recvbuf[6];
				bufferackff[9] = recvbuf[7];
				sendto(sclient, (LPCSTR)bufferackff, sizeof(bufferackff), 0, (sockaddr *)&addrClient, len); //回应
				bufferackff[9] = 0xfe;
				sendto(sclient, (LPCSTR)bufferackff, sizeof(bufferackff), 0, (sockaddr *)&addrClient, len); //回应
			}
			//fclose(FSPOINTER);
			//fopen_s(&FSPOINTER, filepath2, "ab+");
//			int nu;

			while (i<((((unsigned char)recvbuf[2] - 0x00) * 16 * 16 + ((unsigned char)recvbuf[3] - 0x00)) + 4)) {
				//t = 1;
				//1032是最大数据长度 
				/*if (bytecount % 10000==0&&bytecount !=0) {
				t++;
				}*/
				if (bytecount == 3000) {
					//t表示已写文件数
					t += 1;
					//std::cout << t << "个\n" ;
					filescount += 1;
					if (filescount == 9) {
						filescount = 0;
						//goto loop; 
					}
					bytecount = 0;
					readb[(filescount % 9)][bytecount] = (unsigned char)recvbuf[i];

					goto ii;
				}

				readb[(filescount % 9)][bytecount] = (unsigned char)recvbuf[i];
			ii:
				i++;
				bytecount += 1;
			}

		}



		//记录视频帧数量，发送确认帧
		//视频帧头f1 d0 04 04 d1 01 00 a9（视频帧的序号）
		//确认帧格式：f1 d1 00 16（后面帧长度） d1 01 00 09（收到的帧的数量） 00 a6 00 a7 00 a8 00 a9 00 aa 00 ab 00 ac 00 ad 00 ae（帧编号）

	}
//loop://fclose(FSPOINTER);
	 //beforechange();
	closesocket(sclient);
	WSACleanup();

}
int sdl2player()

{
	time_t now_time;
	now_time = time(NULL);
	AVCodecID codec_id = AV_CODEC_ID_H264;
	AVCodecParserContext *pCodecParserCtx = NULL;
	AVFormatContext   *pFormatCtx = NULL;
	AVCodec* pDec = NULL;
	AVCodecContext* pDecCtx = avcodec_alloc_context3(pDec);
	AVPacket* packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	AVFrame *pFrame = av_frame_alloc();
	AVFrame *pFrameYUV = av_frame_alloc();
	// Register all formats and codecs注册所有格式和编解码器
	av_register_all();
	
	pDec = avcodec_find_decoder(AV_CODEC_ID_H264);
	// Initalizing to NULL prevents segfaults!初始化为空防止分段错误！
	

	/* create decoding context */
	pCodecParserCtx = av_parser_init(codec_id); //初始化 AVCodecParserContext

	//Initialize the AVCodecContext to use the given AVCodec.初始化AVCODEVIEW上下文以使用给定的AVCODEC。
	if (avcodec_open2(pDecCtx, pDec, NULL) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot open video decoder\n");
		return -1;
	}

	
	av_init_packet(packet);
	//Allocate an AVFrame and set its fields to default values.分配一个AVFrand并将其字段设置为默认值。
	
	av_parser_parse2(
		pCodecParserCtx, pDecCtx,
		&(packet->data), &(packet->size),
		readb[0], 3000,
		AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
	//cout << screen_w;
	// Determine required buffer size and allocate buffer确定所需缓冲区大小并分配缓冲区
	int byteSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
		pixel_w,
		pixel_h,
		1
	);
	uint8_t* buffer = (uint8_t *)av_malloc(byteSize);

	// Assign appropriate parts of buffer to image planes in pFrameRGB 在PFRAMROGB中为图像平面分配适当的缓冲区
	// Note that pFrameRGB is an AVFrame, but AVFrame is a superset 
	// of AVPicture
	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize,
		buffer, AV_PIX_FMT_YUV420P,
		pixel_w,
		pixel_h,
		1
	);

	int screen_w = pixel_w ;
	//cout << screen_w;
	int screen_h = pixel_h ;
	// initialize SWS context for software scaling
	//cout << "ff";
	SwsContext *pSws_ctx = sws_getContext(pixel_w,
		pixel_h,
		AV_PIX_FMT_YUV420P,
		screen_w,
		screen_h,
		AV_PIX_FMT_YUV420P,
		SWS_BICUBIC,
		NULL,
		NULL,
		NULL
	);
	//cout << now_time;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}
	
	//SDL 2.0 Support for multiple windows  
	SDL_Window* screen = SDL_CreateWindow("SDL",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		screen_w, screen_h,
		SDL_WINDOW_OPENGL
	);

	if (!screen)
	{
		printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
		return -1;
	}

	SDL_Renderer* sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
	//IYUV: Y + U + V  (3 planes)  
	SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer,
		SDL_PIXELFORMAT_IYUV,
		SDL_TEXTUREACCESS_STREAMING,
		screen_w,
		screen_h
	);
	SDL_Rect sdlRect;
	sdlRect.x = 0;
	sdlRect.y = 0;
	sdlRect.w = screen_w;
	sdlRect.h = screen_h;
	int got_picture;
	int ret = -1;
	SDL_Event event;
	int lily_size;
	while (1)
	{
		
	pool:
		if (r < t)
		{
			r++;
		}
		else goto pool;
		uint8_t *lily = readb[filecount % 9];
		lily_size = 3000;
		while (lily_size>0) {
			int len = av_parser_parse2(
				pCodecParserCtx, pDecCtx,
				&(packet->data), &(packet->size),
				lily, lily_size,
				AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
			
			lily += len;
			lily_size -= len;
			
			if (packet->size == 0)
			{
				filecount++;
				continue;
			}
			/**
			*Technically a packet can contain partial frames or other bits of data,技术上，数据包可以包含部分帧或其他数据位，
			*but ffmpeg's parser ensures that the packets we get contain either complete or multiple frames.但是FFMPEG的解析器确保我们得到的数据包包含完整的或多个帧。
			*/
			ret = avcodec_decode_video2(pDecCtx, pFrame, &got_picture, packet);
			if (ret < 0)
			{
				break;
			}

			///*if (packet->stream_index == video_stream_index)
			//{*/
			//	//Supply raw packet data as input to a decoder.将原始分组数据作为输入提供给解码器

			//	//printf("read video frame, timestamp =%lld \n", packet->pts);
			//	ret = avcodec_send_packet(pDecCtx, packet);

			//	//printf("read video frame, timestamp =%lld \n", packet->pts);
			//	if (ret < 0)
			//	{
			//		av_log(NULL, AV_LOG_ERROR, "Error while sending a packet to the decoder\n");
			//		break;
			//	}

				//while (ret >= 0)
			//	{
					//Return decoded output data from a decoder.
			if (first_time) {
				//SwsContext
				pSws_ctx = sws_getContext(screen_w, pixel_h, AV_PIX_FMT_YUV420P,
					screen_w, pixel_h, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

				pFrameYUV = av_frame_alloc();
				//out_buffer = 
				avpicture_fill((AVPicture *)pFrameYUV, (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, screen_w, pixel_h)), AV_PIX_FMT_YUV420P, screen_w, pixel_h);

				//y_size = screen_w* pixel_h;

				first_time = 0;
			}
					//ret = avcodec_receive_frame(pDecCtx, pFrame);//一个包可能有多个帧
					if (ret >= 0)
					{
						// Convert the image from its native format to YUV 将图像从原生格式转换为YUV
						sws_scale(pSws_ctx,
							(const uint8_t* const *)pFrame->data,
							pFrame->linesize,
							0,
							pixel_h,
							pFrameYUV->data,
							pFrameYUV->linesize
						);
						cout << pFrame->pts;

					}
					else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
					{
						break;
					}
					else
					{
						av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
						goto end;
					}
					//cout << system("time");
					//cout << time(NULL);
					SDL_UpdateYUVTexture(sdlTexture, &sdlRect,
						pFrameYUV->data[0], pFrameYUV->linesize[0],
						pFrameYUV->data[1], pFrameYUV->linesize[1],
						pFrameYUV->data[2], pFrameYUV->linesize[2]
					);
					SDL_RenderClear(sdlRenderer);
					SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
					SDL_RenderPresent(sdlRenderer);
			//	}

			}
			// Wipe the packet.		
			av_packet_unref(packet);
			SDL_PollEvent(&event);
			switch (event.type)
			{
			case SDL_QUIT:
				SDL_Quit();
				break;
			default:
				break;
			}
		//}
	}
end:
	// Free the packet that was allocated by av_read_frame
	av_packet_free(&packet);
	//Free the swscaler context swsContext
	sws_freeContext(pSws_ctx);
	//Free the codec context and everything associated with it and write NULL to the provided pointer.
	avcodec_free_context(&pDecCtx);
	// Close the video file
	avformat_close_input(&pFormatCtx);
	//Free the frame and any dynamically allocated objects in it
	av_frame_free(&pFrame);
	av_frame_free(&pFrameYUV);
	// Free the RGB image
	av_free(buffer);
	return 0;
}
int main() {
	boost::thread thrd(recvw);
	thrd.detach();
	//Sleep(1000);
	//recvw();
//loop:
	//if (t > 0) {
		std::cout << "hhhhhhhhhhhhhh";
		boost::thread thrd2(sdl2player);
		thrd2.detach();
	//}
	//else goto loop;
	//SDLplay();
	while (1) {
		std::cin >> n;
		if (n == 9) { break; }
	}
	return 0;
}