#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <libnet.h>
#define THREAD_NUM 100

pthread_t thread[THREAD_NUM];
struct parameter{
char *destination_ip_str;/* 目的IP地址字符串 */
char *source_ip_str; /* 源IP地址字符串 */
};
void *mythread(void * arg)
{
    int i;
    struct parameter *pstru ;
    pstru=(struct parameter *)arg;
    libnet_t *l = NULL;/* libnet句柄 */
    libnet_ptag_t protocol_tag;/* 协议标记 */
    char *payload_liu_wen_tao = NULL; /* 负载 */
    u_short payload_length = 0; /* 负载长度 */
    char *device = "eth0";/*网络设备接口*/

    u_long source_ip = 0; /* 源IP地址 */
    u_long destination_ip = 0; /* 目的IP地址 */
    char errbuf[LIBNET_ERRBUF_SIZE]; /* 错误信息 */
    int packet_length; /* 发送的数据包的长度 */
    l = libnet_init( /* 初始化libnet */
    LIBNET_RAW4, /* libnet类型，为原始套接字IPv4类型 */
    device,  /* 网络设备接口 */errbuf /* 错误信息 */
    );
    source_ip = libnet_name2addr4(l, pstru->source_ip_str, LIBNET_RESOLVE);
    /* 把源IP地址字符串形式转化为网络字节顺序的数据 */
    destination_ip = libnet_name2addr4(l, pstru->destination_ip_str, LIBNET_RESOLVE);
    /* 把目的IP地址字符串形式转化为网络字节顺序的数据 */
    protocol_tag = libnet_build_icmpv4_echo( /* 构造ICMP回显数据包 */
    ICMP_ECHO, /* 类型，此时为回显请求 */
    0,/* 代码，应该为0 */
    0, /* 校验和，为0，表示由libnet句柄自动计算 */
    123,  /* 标识符，赋值为123，自己任意填写数值 */
    456, /* 序列号，赋值为245，自己任意填写数值 */
    NULL,  /* 负载，赋值为空 */
    0, /* 负载的长度，赋值为0 */
    l, /* libnet句柄，应该是由libnet_init()函数得到的 */
    0  /* 协议块标记，赋值为0，表示构造一个新的协议块 */
    );
    protocol_tag = libnet_build_ipv4(/* 构造IP协议块 */
    LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H + payload_length,/* IP协议块的长度 */
    0, /* 服务质量，这里赋值为0 */
    10,  /* 标识符，这里赋值为10 */
    0, /* 偏移，这里赋值为0 */
    20,/* 生存时间，这里赋值为20 */
    IPPROTO_ICMP,/* 上层协议类型，这里是ICMP协议 */
    0, /* 校验和，这里为0表示由libnet计算校验和 */
    source_ip, /* 源IP地址 */
    destination_ip,/* 目的IP地址 */
    payload_liu_wen_tao, /* 负载 */
    payload_length, /* 负载的长度 */
    l,/* libnet句柄 */
    0 /* 协议块标记，为0表示构造一个新的IP协议块 */
    );
    while (1){
        packet_length = libnet_write(l); /* 发送由libnet句柄l表示的数据包 */
        printf("the length of the ICMP packet is %d\n", packet_length);
        /* 输出发送的数据包信息 */
        }
    libnet_destroy(l); /* 销毁libnet */
}

void thread_wait(void)
{
        /*等待线程结束*/
    int i;
    for(i=0;i<THREAD_NUM;i++)
        if(thread[i] !=0) {
                pthread_join(thread[i],NULL);
                //printf("线程%d已经结束\n",i+1);
        }

}
int main(int argc,char *argv[])
{
    struct parameter p;
    p.source_ip_str=argv[1];
    p.destination_ip_str=argv[2];
    printf("main:%s %s \n",p.source_ip_str,p.destination_ip_str);
    int temp,i;
        memset(&thread, 0, sizeof(thread));
        /*创建线程*/
    for(i=0;i<THREAD_NUM;i++)
        if((temp = pthread_create(&thread[i],NULL,mythread,&p)) != 0)
                printf("线程创建失败!\n");

        thread_wait();
        return 0;
}