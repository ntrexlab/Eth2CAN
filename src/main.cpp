#include "main.h"

int RecvPacket(int client_socket, long *ID, int *length, char data[8], int *Ext, int *RTR);
int SendPacket(int client_socket, long ID, int length, char data[8], int Ext, int RTR);

int main(void)
{
    char rdata[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    long rid;
    int rlen, ext, rtr;

    // 소켓을 생성합니다.
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        std::cerr << "소켓 생성 오류" << std::endl;
        return 1;
    }

    // 서버에 연결하기 위한 sockaddr_in 구조체를 설정합니다.
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    // 서버에 연결합니다.
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        std::cerr << "서버 연결 오류" << std::endl;
        close(client_socket);
        return 1;
    }

    std::cerr << "서버 연결 성공" << std::endl;
    std::cerr << "SERVER_IP : " << SERVER_IP << std::endl;
    std::cerr << "SERVER_PORT : " << SERVER_PORT << std::endl;

    // 서버로 "T=1" 문자열을 전송합니다.
    const char *message = "T=1\r\n";
    send(client_socket, message, strlen(message), 0);
    std::cout << "서버에 메시지를 전송했습니다: " << message << std::endl;

    for (int i=0; i<1000; i++) {
		long sid = 5;
		unsigned char sdata[8] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11 };

    int ret = SendPacket(client_socket, sid, 8,  (char *)sdata, 0, 0);

    if(ret) printf("서버에 데이터를 전송했습니다. \r\n");
    }


    /*
    while (true)
    {
       int ret2 = RecvPacket (client_socket, &rid, &rlen, rdata, &ext, &rtr);

       if(ret2)
       {
        printf("ID : %ld length : %d Data : [%02x] [%02x] [%02x] [%02x] [%02x] [%02x] [%02x] [%02x] \r\n",rid,rlen,rdata[0],rdata[1],rdata[2],rdata[3],rdata[4],rdata[5],rdata[6],rdata[7]);
       }
    }
    */

    return 0;
}

char Checksum (char *data, int len)
{
	char cs = 0;
	for (int i=0; i<len; i++) cs += data[i];

	return cs;
}

int SendPacket(int client_socket, long ID, int length, char data[8], int Ext, int RTR)
{
    unsigned char buff[18];

    buff[0] = 0x02;
    buff[1] = 0x00;
    buff[2] = (unsigned char)length;

    Ext==1?buff[3]=0x40:buff[3]=0x00;

    buff[4] = ID;
    buff[5] = ID >> 8;
    buff[6] = ID >> 16;
    buff[7] = ID >> 24;

    memcpy(&data[0], &buff[8], length);

    buff[16] = Checksum((char*)&buff[1], 15);

    buff[17] = 0x03;

    // 서버에 데이터를 전송합니다.
    int ret = send(client_socket, buff, sizeof(buff), 0);

    return ret;
}

int RecvPacket(int client_socket, long *ID, int *length, char data[8], int *Ext, int *RTR)
{
    long CAN_ID;

    char buffer[1024];

    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

    if (bytes_received == -1) return -1;

    else if (bytes_received == 0) return -1;

    else if (buffer[0] != 0x02) return -1;

    else if (buffer[17] != 0x03) return -1;

    else if (buffer[1] != 0x00) return -1; // 메뉴얼을 보면 Message Packet는 0x00 으로 고정되어있다.

    else if (buffer[16] != Checksum(&buffer[1], 15)) return -1; // Checksum 확인

    CAN_ID = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);

    *ID = CAN_ID;
    *length = (int)buffer[2];

    memcpy(data, &buffer[8], 8);

    return 1;
}


