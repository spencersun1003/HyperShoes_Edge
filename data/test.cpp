#include <iostream>
#include <queue>
using namespace std;

// void queue_safepush(queue<float> &data,float reading,int maxlength){
//     data.push(reading);
//     if (data.size() > maxlength){
//         data.pop();
//     }
// }
class test
{
private:
    
    /* data */
public:
    int a;
    test(int init_a);
    ~test();
};

test::test(int init_a)
{
    a = init_a;
}

test::~test()
{
}




int main()
{   queue<float> data0;
    // queue_safepush(data0,11.2,3);
    // queue_safepush(data0,11.3,3);
    // queue_safepush(data0,11.4,3);
    // queue_safepush(data0,11.5,3);
    cout<<data0.front()<<endl;

}
