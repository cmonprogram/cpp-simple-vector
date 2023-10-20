# Advanced-vector
Простой контейнер вектор
***

## Развертывание
```
g++ main.cpp array_ptr.h simple_vector.h -o simple_vector -std=c++17 -O3
./simple_vector
```
# Формат входных данных
```
SimpleVector<int> v;   Создает вектор нужного типа
v.PushBack(i);         Добавляет элемент вектора 
v.Erase(v.begin())     Удаляет элемент вектора
```
# Формат выходных данных
```
v[i]                   Получает элемент вектора
v.GetSize()            Получает размер вектора
```  
## Использование
### Ввод
```
int main() {
    SimpleVector<int> v;
    for (size_t i = 0; i < 10; ++i) {
        v.PushBack(i);
    }
    assert(v.GetSize() == 10);
    for (size_t i = 0; i < v.GetSize() ; ++i) {
        assert(v[i] == i);
        std::cout << v[i] << endl;
    }
    auto it = v.Erase(v.begin());
    assert(v.GetSize()  == 9);
}
```
### Вывод
```
0
1
2
3
4
5
6
7
8
9
```
