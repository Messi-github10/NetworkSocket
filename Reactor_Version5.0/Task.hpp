#ifndef TASK_HPP
#define TASK_HPP

#include <functional>
using namespace std;

// ​定义一个通用的任务类型别名，用于表示无参数且无返回值的可调用对象
using Task = function<void()>;

#endif