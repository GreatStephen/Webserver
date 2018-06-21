#include<WinSock2.h>
#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include<pthread.h>
#include<string>
#include<fstream>

using namespace std;

#pragma comment(lib,"WS2_32.LIB")
#pragma comment(lib,"pthreadVC2.LIB")


void* conn_proc(void* args) {
	//���߳�
	SOCKET sockConn = *(SOCKET*)args;
	int i, j, index=0;
	

	char recvBuf[100];
	recv(sockConn, recvBuf, 100, 0);//�������ӿͻ��˽�������
	printf("�������\n");
	printf("%s\n", recvBuf);
	//closesocket(sockConn);//�ر�socket

	//����������
	char method[5];
	char addr[50];//����·��
	char abs_addr[50];//����·��
	char *type = (char*)malloc(5);//content-type
	char *filetype = (char*)malloc(5);//ʵ���ļ�����
	char *HTMLcontent = (char*)malloc(400);
	char *TXTcontent = (char*)malloc(400);
	char *ICOcontent = (char*)malloc(6000);
	char *JPGcontent = (char*)malloc(16000);
	char *temp = (char*)malloc(100);
	char *responseBuffer = (char*)malloc(500);
	int length_int;
	char length[8];//buffer����
	fstream icofile;
	fstream htmlfile;
	fstream jpgfile;

	for (i = 0;i<sizeof(recvBuf); i++) {
		if (recvBuf[i] != ' ') method[i] = recvBuf[i];
		else break;
	}
	method[i] = '\0';

	if (strcmp(method, "POST") == 0) {
		//POST����
		printf("post����\n");
	}

	else if (strcmp(method, "GET") == 0) {
		//GET����
		printf("get����\n");

		//�ļ�·��
		if (recvBuf[i] != ' ') exit(0);
		for (i = i + 1,j=0;; i++,j++) {
			if (recvBuf[i] != ' ') addr[j] = recvBuf[i];
			else {
				addr[j] = '\0';
				break;
			}
		}
		//printf("addr%s\n", addr);

		//ӳ��Ϊ���������ļ��ľ���·��
		for (j = 0; j<50; j++) if (addr[j] == '/') index=j;
		abs_addr[0] = 'E';
		abs_addr[1] = ':';
		for (j = 2;; j++,index++) {
			if (addr[index] == '\0') {
				abs_addr[j] = '\0';
				break;
			}
			abs_addr[j] = addr[index];
		}
		printf("absaddr%s\n", abs_addr);

		//����content-type
		for (j = 0; j < 50; j++) {
			if (abs_addr[j] == '.') {
				if (abs_addr[j + 1] == 't') {
					type = "text/html";
					filetype = "txt";
				}
				else if (abs_addr[j + 1] == 'h') {
					type = "text/html";
					filetype = "html";
				}
				else if (abs_addr[j + 1] == 'i') {
					type = "image/x-ico";
					filetype = "ico";
				}
				else if (abs_addr[j + 1] == 'j') {
					type = "application/x-jpg";
					filetype = "jpg";
				}
				break;
			}
		}
		//printf("type=%s\n", type);


		//���ļ�
		if (strcmp(type, "image/x-ico") == 0) icofile.open(abs_addr, ios::binary | ios::in);
		else if (strcmp(type, "application/x-jpg") == 0) jpgfile.open(abs_addr, ios::binary | ios::in);
		else htmlfile.open(abs_addr, ios::in| ios::binary);//һ��Ҫ�ö����ƴ�
		if (strcmp(type, "text/html") == 0 && !htmlfile.is_open() ) {
			//�Ҳ���html�ļ�������404
			printf("cannot find file.\n");
			strcpy(responseBuffer, "HTTP/1.1 ");
			strcat(responseBuffer, "404 ");
			strcat(responseBuffer, "NOT FOUND\r\n");
			strcat(responseBuffer,"\0");
			send(sockConn, responseBuffer, 501, 0);
			closesocket(sockConn);//�ر�socket
			return NULL;
		}
		else if (strcmp(type, "image/x-ico") == 0 && !icofile.is_open()) {
			//�Ҳ���ico�ļ�������404
			printf("cannot find file.\n");
			strcpy(responseBuffer, "HTTP/1.1 ");
			strcat(responseBuffer, "404 ");
			strcat(responseBuffer, "NOT FOUND\r\n");
			strcat(responseBuffer, "\0");
			send(sockConn, responseBuffer, 501, 0);
			closesocket(sockConn);//�ر�socket
			return NULL;
		}
		else if (strcmp(type, "application/x-jpg") == 0 && !jpgfile.is_open()) {
			//�Ҳ���jpg�ļ�������404
			printf("cannot find file.\n");
			strcpy(responseBuffer, "HTTP/1.1 ");
			strcat(responseBuffer, "404 ");
			strcat(responseBuffer, "NOT FOUND\r\n");
			strcat(responseBuffer, "\0");
			send(sockConn, responseBuffer, 501, 0);
			closesocket(sockConn);//�ر�socket
			return NULL;
		}

		//���ļ�����д���ı�����������
		if (strcmp(type, "text/html") == 0 && strcmp(filetype,"html")==0) {
			//html�ļ�
			strcpy(HTMLcontent, "\0");
			htmlfile.seekg(0, ios::end);
			length_int = htmlfile.tellg();
			htmlfile.seekg(0, ios::beg);
			
			//printf("length_int=%d\n", length_int);
			htmlfile.read(HTMLcontent, length_int);
			strcat(HTMLcontent, "\0");
			//printf("start:%s\n", HTMLcontent);
			
			itoa(length_int, length, 10);
			htmlfile.close();
		}
		else if (strcmp(type, "text/html") == 0 && strcmp(filetype, "txt") == 0) {
			//txt�ļ�

		}
		else if (strcmp(type, "image/x-ico") == 0 && strcmp(filetype, "ico") == 0) {
			//ico�ļ�
			strcpy(ICOcontent, "\0");
			icofile.seekg(0, ios::end);
			length_int = icofile.tellg();
			icofile.seekg(0, ios::beg);
			//printf("length_int=%d\n", length_int);
			icofile.read(ICOcontent, length_int);
			strcat(ICOcontent, "\0");
			//printf("start:%s\n", ICOcontent);
			itoa(length_int, length, 10);
			icofile.close();
		}
		else if (strcmp(type, "application/x-jpg") == 0 && strcmp(filetype, "jpg") == 0) {
			//jpg�ļ�
			strcpy(JPGcontent, "\0");
			jpgfile.seekg(0, ios::end);
			length_int = jpgfile.tellg();
			jpgfile.seekg(0, ios::beg);
			printf("length_int=%d\n", length_int);
			//jpgfile.read(JPGcontent, length_int);

			char* c = (char*)malloc(1);
			while (1) {
				if (jpgfile.eof()) break;
				jpgfile.read(c, 1);
				strcat(JPGcontent, c);
				jpgfile.seekg(1, ios::cur);
			}

			strcat(JPGcontent, "\0");
			printf("start:%p\n", JPGcontent);
			itoa(length_int, length, 10);
			jpgfile.close();
		}
		

		//printf("%s\n", HTMLcontent);

		//��HTTP��Ӧ����
		strcpy(responseBuffer, "HTTP/1.1 ");
		strcat(responseBuffer, "200 ");
		strcat(responseBuffer, "OK\r\n");
		strcat(responseBuffer, "Content-Length:");
		strcat(responseBuffer, length);
		strcat(responseBuffer, "\r\n");
		strcat(responseBuffer, "Connection:close\r\n");
		strcat(responseBuffer, "Content-Type:");
		strcat(responseBuffer, type);
		strcat(responseBuffer, "\r\n\r\n");
		if(strcmp(type,"text/html")==0) strcat(responseBuffer, HTMLcontent);
		else if (strcmp(type, "image/x-ico") == 0) strcat(responseBuffer, ICOcontent);
		else if (strcmp(type, "application/x-jpg") == 0) strcat(responseBuffer, JPGcontent);
		strcat(responseBuffer, "\0");

		int reslength = 0;
		while (*(responseBuffer + reslength) != '\0') {
			reslength++;
		}
		reslength++;

		send(sockConn, responseBuffer, reslength, 0);
		printf("%s\n", responseBuffer);

	}


	closesocket(sockConn);//�ر�socket
	return NULL;
}



int main() {
	//�������߳�
	
	pthread_t thread[20];//�������20������
	int n = 0;
	
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);//�ú����Ĺ����Ǽ���һ��Winsocket��汾
	if (err != 0) {
		return NULL;
	}
	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		return NULL;
	}
	//����socket��̲���
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);//�������ӵĿɿ��Է���SOCK_STRAM
	SOCKADDR_IN addrSrv;//��ű��ص�ַ��Ϣ��
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//htol�������ֽ���long��ת��Ϊ�����ֽ���
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(4790);//htos�������˿�ת�����ַ���1024���ϵ����ּ���
	bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));//��socket�󶨵���Ӧ��ַ�Ͷ˿���
	listen(sockSrv, 5);//�ȴ������е���󳤶�Ϊ5
	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);

	while (1)
	{
		printf("��ʼ����4790�˿�\n");
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);//����һ���µ��׽�������ͨ�ţ�����ǰ��ļ����׽���
		/*
		char sendBuf[100];
		printf(sendBuf, "Server IP is:%s",
			inet_ntoa(addrClient.sin_addr));//inet_nota�����ǽ��ַ�ת����ip��ַ
		send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);//��������ͻ��˷�������

		char recvBuf[100];
		recv(sockConn, recvBuf, 100, 0);//�������ӿͻ��˽�������
		printf("%s\n", recvBuf);
		closesocket(sockConn);//�ر�socket
		*/
		int err = pthread_create(&thread[n++], NULL, conn_proc, (void*)(&sockConn));
		if (err) {
			printf("conn_proc error!\n");
		}
	}
	
}


