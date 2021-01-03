/*
	�ɼ���Ƶ�Ĳ���
	1. ע���豸

	2. ���òɼ���ʽ

	3. ����Ƶ�豸

	4. ��ȡ��Ƶ����

	��ע�����ִ��ʱ���Ҳ�����Ĵ���vi /etc/profile������export LD_LIBRARY_PATH="/usr/local/ffmpeg/lib"��
		 ����ִ��ֱ��export���ָ��ld���ӿ��·�����޸�/etc/profile����������Ч��ִ��exportֻ�Ե�ǰ�û�������Ч
*/


#include <stdio.h>
extern "C"
{
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}


#define DEV_NAME_MAX_LEN  1024
#define ERR_BUF_MAX_LEN   1024


int main(void)
{
	int nRet = 0;
	int nRdCnt = 0;
	AVPacket tAvPkt;
	AVInputFormat* ptAudFmt = NULL;
	AVFormatContext* ptAvFmtCtx = NULL;
	char s8DevName[DEV_NAME_MAX_LEN] = { 0 };
	char s8ErrBuf[ERR_BUF_MAX_LEN] = { 0 };

	// 0. ���ô�ӡ�ȼ�
	av_log_set_level(AV_LOG_DEBUG);

	// 1. ע���豸
	avdevice_register_all();

	// 2. ���òɼ���ʽ��linux�²ɼ���ʽΪalsa��mac��Ϊavfoundation��windowsΪdshow
	ptAudFmt = av_find_input_format("alsa");
	if (NULL == ptAudFmt)
	{
		av_log(NULL, AV_LOG_DEBUG, "av_find_input_format alsa failed\n");
		//printf("av_find_input_format alsa failed\n");
		return -1;
	}

	// 3. ����Ƶ�豸��ע��������豸���ƣ���ͬƽ̨�¸�ʽ��һ��
	strncpy(s8DevName, "hw:0, 0", sizeof(s8DevName));

	// ��avformat_close_input�ɶ�ʹ�ã����������ڴ�й©
	nRet = avformat_open_input(&ptAvFmtCtx, s8DevName, ptAudFmt, NULL);
	if (0 != nRet)
	{
		av_strerror(nRet, s8ErrBuf, ERR_BUF_MAX_LEN);
		//printf("avformat_open_input failed, error %s\n", s8ErrBuf);
		av_log(NULL, AV_LOG_DEBUG, "avformat_open_input failed, error %s\n", s8ErrBuf);
		return -1;
	}
	else
	{
		av_log(NULL, AV_LOG_DEBUG, "avformat_open_input succ\n");
		//printf("avformat_open_input succ\n");
	}

	// 4. ��ȡ��Ƶ����
	av_init_packet(&tAvPkt);    // ��ʼ����������av_packet_unref�ɶ�ʹ��

	while (1)
	{
		if (nRdCnt > 100)
		{
			break;
		}

		nRet = av_read_frame(ptAvFmtCtx, &tAvPkt);
		if (nRet != 0)
		{
			av_strerror(nRet, s8ErrBuf, ERR_BUF_MAX_LEN);
			av_log(NULL, AV_LOG_DEBUG, "av_read_frame failed, error %s\n", s8ErrBuf);
			//printf("av_read_frame failed, error %s\n", s8ErrBuf);
			break;
		}
		else
		{
			av_packet_unref(&tAvPkt); // ע��:ÿ��read�󣬶���Ҫ�ͷ���Դ
			//printf("read succ. frame_size:%d\n", tAvPkt.size);
			av_log(NULL, AV_LOG_DEBUG, "read succ. frame_size:%d\n", tAvPkt.size);
		}

		nRdCnt++;
	}
	
	av_packet_unref(&tAvPkt);			// ������Դ
	avformat_close_input(&ptAvFmtCtx);  // ����������

	return 0;
}