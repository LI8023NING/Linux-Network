#include"comm.h"

static int com_msg_queue(int flags)
{
    key_t _key = ftok(_PROJ_NAME_,_PROJ_ID_);
    if(_key < 0)
    {
	perror("ftok");
	return -1;
    }

    int msg_id = msgget(_key,flags); //
    if(msg_id < 0)
    {
	perror("msgget");
    }

    return msg_id;
}

int create_msg_queue()
{
    int flags = IPC_CREAT | IPC_EXCL | 0644;
    return com_msg_queue(flags);
}

int get_msg_queue()
{
    int flags = IPC_CREAT;
    return com_msg_queue(flags);
}

int destroy_msg_queue(int msg_id)
{
    if(msgctl(msg_id,IPC_RMID,NULL) < 0)
    {
	perror("msgctl");
	return -1;
    }
    return 0;
}

int send_msg(int msg_id,int t,const char* msg)
{
    struct msgbuf _msg;
    _msg.mtype = t;
    strncpy(_msg.mtext,msg,strlen(msg)+1);

    if(msgsnd(msg_id,&_msg,sizeof(_msg.mtext),0) < 0)
    {
	perror("msgsnd");
	return -1;
    }
    return 0;
}

int recv_msg(int msg_id,int t,char* out)
{
    struct msgbuf _msg;
    _msg.mtype = t;
    memset(_msg.mtext,'\0',sizeof(_msg.mtext));
    if(msgrcv(msg_id,&_msg,sizeof(_msg.mtext),t,0) < 0)
    {
	perror("msgrcv");
	return -1;
    }
    else
    {
	strcpy(out,_msg.mtext);
    }
    return 0;
}
