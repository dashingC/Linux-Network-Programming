#include<iostream>
#include<string>
#include<pthread.h>
#include <unistd.h>

int tickets = 1000;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;

void* start_routine(void* arg)
{
    std::string name = static_cast<const char*>(arg);
    while (true)
    {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond,&mutex);
        std::cout << name << " ->" << tickets<<std::endl;
        tickets--;
        pthread_mutex_unlock(&mutex);


    }
}

int main()
{
    pthread_t t[5];
    for(int i=0;i<5;i++)
    {
        char *name=new char[10];
        snprintf(name,10,"thread %d",i+1);
        pthread_create(t+i,nullptr,start_routine,name);

    }
   

    while(true)
    {
        sleep(1);
        // pthread_cond_signal(&cond);
        pthread_cond_broadcast(&cond);
    }

    for(int i=0;i<5;i++)
    {
        pthread_join(t[i],nullptr);
    }

}