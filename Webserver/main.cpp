#include<WinSock2.h>
#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include<pthread.h>
#include<string>
#include<fstream>
#include<iostream>

using namespace std;

#pragma comment(lib,"WS2_32.LIB")
#pragma comment(lib,"pthreadVC2.LIB")


void* conn_proc(void* args) {
	//���߳�
	SOCKET sockConn = *(SOCKET*)args;
	int i, j, index=0;
	

	char recvBuf[1000];
	memset(recvBuf, 0, 1000);
	recv(sockConn, recvBuf, 1000, 0);//�������ӿͻ��˽�������
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
	//char *ICOcontent = (char*)malloc(6000);
	char ICOcontent[6000];
	char JPGcontent[16000];
	char *temp = (char*)malloc(100);
	char *responseBuffer = (char*)malloc(20000);
	int length_int;
	char length[8];//buffer����
	//fstream icofile;
	fstream htmlfile;
	fstream txtfile;
	//fstream jpgfile;
	FILE* jpg_file=NULL;
	FILE* ico_file = NULL;

	for (i = 0;i<sizeof(recvBuf); i++) {
		if (recvBuf[i] != ' ') method[i] = recvBuf[i];
		else break;
	}
	method[i] = '\0';
	//cout << "method=" << method << endl;

	if (strcmp(method, "POST") == 0) {
		//POST����
		printf("post����\n");
		//�ļ�·��
		if (recvBuf[i] != ' ') cout << "error"<<endl;
		for (i = i + 1, j = 0;; i++, j++) {
			if (recvBuf[i] != ' ') addr[j] = recvBuf[i];
			else {
				addr[j] = '\0';
				break;
			}
		}
		//cout << "addr=" << addr << endl;
		for (j = 0; j<50; j++) if (addr[j] == '/') index = j;
		*abs_addr = '\0';
		strcat(abs_addr, &addr[index + 1]);

		if (strcmp(abs_addr, "dopost") != 0) {
			//dopost�����󣬷���404
			strcpy(responseBuffer, "HTTP/1.1 ");
			strcat(responseBuffer, "404 ");
			strcat(responseBuffer, "NOT FOUND\r\n");
			strcat(responseBuffer, "\0");
			send(sockConn, responseBuffer, 501, 0);
			closesocket(sockConn);//�ر�socket
			return NULL;
		}
		else {
			//�ҵ�login
			for (i = 0; i < 995; i++) {
				//cout << "once" << endl;
				if (recvBuf[i] == 'l' &&
					recvBuf[i + 1] == 'o' &&
					recvBuf[i + 2] == 'g' &&
					recvBuf[i + 3] == 'i' &&
					recvBuf[i + 4] == 'n')
					break;
			}
			char* username = (char*)malloc(20);
			char* password = (char*)malloc(20);
			//��ȡlogin
			int n = 0;
			for (i = i + 6;; i++) {
				if (recvBuf[i] != '&') {
					*(username + n++) = recvBuf[i];
				}
				else break;
			}
			*(username + n) = '\0';
			//cout << "username:" << username << endl;
			//��ȡpass
			n = 0;
			for (i = i + 1;; i++) {
				if (recvBuf[i] == 'p' &&
					recvBuf[i + 1] == 'a' &&
					recvBuf[i + 2] == 's' &&
					recvBuf[i + 3] == 's' &&
					recvBuf[i + 4] == '=')
					break;
			}
			for (i = i + 5;; i++) {
				if (recvBuf[i] != 0) {
					*(password + n++) = recvBuf[i];
				}
				else break;
			}
			*(password + n) = '\0';

			//cout << "password:" << password << endl;

			//�ж�login��pass�Ƿ���ȷ
			char* judge = (char*)malloc(50);
			
			if (strcmp(username, "3150104790") == 0 && strcmp(password, "4790") == 0) {
				strcpy(judge, "login success!");
			}
			else strcpy(judge, "login failed!");
			length_int = 0;
			for (i = 0;; i++) {
				if (*(judge + i) != '\0') length_int++;
				else break;
			}
			//cout << judge << endl;
			

			char* message = (char*)malloc(100);
			strcpy(message, "<html><head><title>login</title></head><body><h1>");
			strcat(message, judge);
			strcat(message, "</h1></body></html>");
			
			length_int = strlen(message);
			itoa(length_int, length, 10);

			strcpy(responseBuffer, "HTTP/1.1 ");
			strcat(responseBuffer, "200 ");
			strcat(responseBuffer, "OK\r\n");
			strcat(responseBuffer, "Content-Length:");
			strcat(responseBuffer, length);
			strcat(responseBuffer, "\r\n");
			strcat(responseBuffer, "Connection:keep-alive\r\n");
			strcat(responseBuffer, "Content-Type:text/html");
			strcat(responseBuffer, "\r\n\r\n");
			strcat(responseBuffer, message);

			
			int result = send(sockConn, responseBuffer, strlen(responseBuffer), 0);
			//cout << "result=" << result << endl;
			if (result == SOCKET_ERROR) {
				printf("SOCKET ERROR!\n");
			}
			//printf("%s\n", responseBuffer);

		}
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
		printf("addr%s\n", addr);

		//ӳ��Ϊ���������ļ��ľ���·��
		for (j = 0; j<50; j++) if (addr[j] == '/') index=j;
		index++;
		abs_addr[0] = 'E';
		abs_addr[1] = ':';
		abs_addr[2] = '/';
		abs_addr[3] = '/';
		for (j = 4;; j++,index++) {
			if (addr[index] == '\0') {
				abs_addr[j] = '\0';
				break;
			}
			abs_addr[j] = addr[index];
		}
		//printf("absaddr%s\n", abs_addr);

		//����content-type
		for (j = 0; j < 50; j++) {
			if (abs_addr[j] == '.') {
				if (abs_addr[j + 1] == 't') {
					type = "text/plain";
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
		//cout << "type=" << type << endl;


		//���ļ�
		if (strcmp(type, "image/x-ico") == 0) {
			ico_file = fopen(abs_addr, "rb");
		}
		else if (strcmp(type, "application/x-jpg") == 0) {
			jpg_file = fopen(abs_addr, "rb");
		}
		else if (strcmp(type, "text/html") == 0) {
			
			htmlfile.open(abs_addr, ios::in | ios::binary);//һ��Ҫ�ö����ƴ�
		}
		else if (strcmp(type, "text/plain") == 0) {
			txtfile.open(abs_addr, ios::in | ios::binary);
		}

		//�ļ��򲻿�������404
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
		else if (strcmp(type, "text/plain") == 0 && !txtfile.is_open()) {
			//�Ҳ���txt�ļ�������404
			printf("cannot find file.\n");
			strcpy(responseBuffer, "HTTP/1.1 ");
			strcat(responseBuffer, "404 ");
			strcat(responseBuffer, "NOT FOUND\r\n");
			strcat(responseBuffer, "\0");
			send(sockConn, responseBuffer, 501, 0);
			closesocket(sockConn);//�ر�socket
			return NULL;
		}
		else if (strcmp(type, "image/x-ico") == 0 && ico_file==NULL) {
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
		else if (strcmp(type, "application/x-jpg") == 0 && jpg_file==NULL) {
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
			
			*(HTMLcontent + length_int) = '\0';
			
			itoa(length_int, length, 10);
			htmlfile.close();
		}
		else if (strcmp(type, "text/plain") == 0 && strcmp(filetype, "txt") == 0) {
			//txt�ļ�
			strcpy(TXTcontent, "\0");
			txtfile.seekg(0, ios::end);
			length_int = txtfile.tellg();
			txtfile.seekg(0, ios::beg);

			//printf("length_int=%d\n", length_int);
			txtfile.read(TXTcontent, length_int);

			*(TXTcontent + length_int) = '\0';

			itoa(length_int, length, 10);
			txtfile.close();
		}
		else if (strcmp(type, "image/x-ico") == 0 && strcmp(filetype, "ico") == 0) {
			//ico�ļ�
			
			fseek(ico_file, 0, SEEK_END);
			length_int = ftell(ico_file);
			fseek(ico_file, 0, SEEK_SET);
			
			cout << length_int << endl;
			
			i = 0;
			
			while (!feof(ico_file)) {
				char ch = fgetc(ico_file);
				//cout << "i=" << i << endl;
				
				*(ICOcontent + i++) = ch;
			}
			*(ICOcontent + i) = '\0';

			itoa(length_int, length, 10);
			fclose(ico_file);
		}
		else if (strcmp(type, "application/x-jpg") == 0 && strcmp(filetype, "jpg") == 0) {
			//jpg�ļ�
			
			fseek(jpg_file, 0, SEEK_END);
			length_int = ftell(jpg_file);
			fseek(jpg_file, 0, SEEK_SET);


			cout << length_int << endl;
			i = 0;

			while (!feof(jpg_file)) {
				char ch = fgetc(jpg_file);
				//cout << "i=" << i << endl;

				*(JPGcontent + i++) = ch;
			}


			itoa(length_int, length, 10);
			fclose(jpg_file);
		}
		

		//��HTTP��Ӧ����
		strcpy(responseBuffer, "HTTP/1.1 ");
		strcat(responseBuffer, "200 ");
		strcat(responseBuffer, "OK\r\n");
		strcat(responseBuffer, "Content-Length:");
		strcat(responseBuffer, length);
		strcat(responseBuffer, "\r\n");
		strcat(responseBuffer, "Connection:keep-alive\r\n");
		strcat(responseBuffer, "Content-Type:");
		strcat(responseBuffer, type);
		strcat(responseBuffer, "\r\n\r\n");

		//indexָ��headerĩβ
		index = 0;
		for (i = 0;; i++) {
			if (*(responseBuffer + i) != '\0') index++;
			else break;
		}
		
		int headlength = strlen(responseBuffer);
		//printf("%s\n", responseBuffer);
		//cout << "headlength=" << headlength << endl;

		if (strcmp(type, "text/html") == 0) {
			strcat(responseBuffer, HTMLcontent);//html���ı�buffer������ֱ�����ַ�������
		}
		else if (strcmp(type, "text/plain") == 0) {
			strcat(responseBuffer, TXTcontent);//txt���ı�buffer������ֱ�����ַ�������
		}
		else if (strcmp(type, "image/x-ico") == 0) {//ͼƬ�������ַ�����������ΪͼƬ���ݿ��ܳ���\0
			for (i = 0; i < length_int; i++) {
				*(responseBuffer + index++) = *(ICOcontent + i);
			}
		}
		else if (strcmp(type, "application/x-jpg") == 0) {//ͼƬ�������ַ�����������ΪͼƬ���ݿ��ܳ���\0
			for (i = 0; i < length_int; i++) {
				*(responseBuffer + index++) = *(JPGcontent + i);
			}
			//cout << "i=" << i << endl;
		}

		//cout << "test:" << headlength + length_int << endl;
		int result=send(sockConn, responseBuffer, headlength+length_int, 0);
		//cout << "num=" << result << endl;
		if (result == SOCKET_ERROR) {
			printf("SOCKET ERROR!\n");
		}
		//printf("%s\n", responseBuffer);

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
		
		int err = pthread_create(&thread[n++], NULL, conn_proc, (void*)(&sockConn));
		if (err) {
			printf("conn_proc error!\n");
		}
	}
	
}


