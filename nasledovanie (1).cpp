#include <iostream>
#include <limits>
#include <memory>

using namespace std;

double eps = numeric_limits<double>::epsilon();

class Value{
public:
    virtual Value* CreateZeroValue() = 0;
    virtual Value* Duplicate() = 0;

    virtual Value* operator+=(const Value&) = 0;
    virtual Value* operator-=(const Value&) = 0;
    virtual Value* operator*=(const Value&) = 0;
    virtual Value* operator/=(const Value&) = 0;
    virtual bool IsZero() const = 0;

    virtual ~Value() {};
};

class RealValue: public Value{
private:
    double value = 0;
public:
    RealValue() = default;
    explicit RealValue(double val) : value(val){};
    Value* CreateZeroValue() override{
        auto *rv = new RealValue(0);
        return rv;
    };
    Value* Duplicate() override{
        auto *rv = new RealValue(value);
        return rv;
    };

    Value* operator+=(const Value& v) override{
        const auto &rv = dynamic_cast<const RealValue&>(v);
        value += rv.value;
        return &*this;
    };
    Value* operator-=(const Value& v) override{
        const auto &rv = dynamic_cast<const RealValue&>(v);
        value -= rv.value;
        return &*this;
    };
    Value* operator*=(const Value& v) override{
        const auto &rv = dynamic_cast<const RealValue&>(v);
        value *= rv.value;
        return &*this;
    };
    Value* operator/=(const Value& v) override{
        const auto &rv = dynamic_cast<const RealValue&>(v);
        if(rv.IsZero())
            throw domain_error("Divide by zero");
        value /= rv.value;
        return &*this;
    };
    bool IsZero() const override{
        return abs(value) < eps;
    }

    double GetValue() const{
        return value;
    }
};

class AdvancedValue: public Value{
private:
    double value = 0, error = 0;
public:
    AdvancedValue() = default;
    AdvancedValue(double val, double err = eps) : value(val), error(err){};
    Value* CreateZeroValue() override{
        auto *av = new AdvancedValue(0);
        return av;
    };
    Value* Duplicate() override{
        auto *av = new AdvancedValue(value);
        return av;
    };

    Value* operator+=(const Value& v) override{
        const auto &av = dynamic_cast<const AdvancedValue&>(v);
        value += av.value;
        error += av.error;
        return &*this;
    };
    Value* operator-=(const Value& v) override{
        const auto &av = dynamic_cast<const AdvancedValue&>(v);
        value -= av.value;
        error += av.error;
        return &*this;
    };
    Value* operator*=(const Value& v) override{
        const auto &av = dynamic_cast<const AdvancedValue&>(v);
        value *= av.value;
        error = abs(value * av.value) * (error / value + av.error / av.value);
        return &*this;
    };
    Value* operator/=(const Value& v) override{
        const auto &av = dynamic_cast<const AdvancedValue&>(v);
        if(av.IsZero())
            throw domain_error("Divide by zero");
        value /= av.value;
        error = abs(value * av.value) * (error / value + av.error / av.value);
        return &*this;
    };
    bool IsZero() const override{
        return abs(value) < eps;
    }

    double GetValue() const{
        return value;
    }
    double GetError() const{
        return error;
    }
};

void Solve (Value** matrix, Value** rhs, Value** x, size_t size){
    int k = 0;
    while (k < size)
    {
        int index = k - 1;
        for (int i = k; i < size; i++)
        {
            if (!matrix[i * size + k]->IsZero())
            {
                index = i;
                break;
            }
        }
        // Перестановка строк
        if (index == k - 1)
        {
            // нет ненулевых диагональных элементов
            cout << "Решение получить невозможно из-за нулевого столбца ";
            cout << index << " матрицы A" << endl;
            return;
        }
        if(index != k) {
            for (int j = 0; j < size; j++) {
                Value *temp = matrix[k * size + j]->Duplicate();
                matrix[k * size + j] = matrix[index * size + j]->Duplicate();
                matrix[index * size + j] = temp->Duplicate();
            }

            Value *temp = rhs[k]->Duplicate();
            rhs[k] = rhs[index]->Duplicate();
            rhs[index] = temp->Duplicate();
        }
        // Нормализация уравнений
        for (int i = k; i < size; i++)
        {
            Value *temp = matrix[i * size + k]->Duplicate();
            if (temp->IsZero()) continue; // для нулевого коэффициента пропустить
            for (int j = k; j < size; j++) {
                *matrix[i * size + j] /= *temp;
            }
            *rhs[i] /= *temp;
            if (i == k)  continue; // уравнение не вычитать само из себя
            for (int j = k; j < size; j++)
                *matrix[i * size + j] -= *matrix[k * size + j];
            *rhs[i] -= *rhs[k];
        }
        k++;
    }
    // обратная подстановка
    for (k = (int)size - 1; k >= 0; k--)
    {
        x[k] = rhs[k]->Duplicate();
        for (int i = 0; i < k; i++){
            *rhs[i] -= *(*matrix[i * size + k]->Duplicate() *= *x[k]);
        }
    }
}

void TestOk(){
    cout << "TestOk" << endl;
    Value* matrix[4];
    Value* rhs[2];
    Value* x[2];
    matrix[0] = new RealValue(-3.0);
    matrix[1] = new RealValue(2.0);
    matrix[2] = new RealValue(-6.0);
    matrix[3] = new RealValue(-9.0);
    rhs[0] = new RealValue(19.0);
    rhs[1] = new RealValue(51.0);
    x[0] = new RealValue();
    x[1] = new RealValue();
    Solve(matrix, rhs, x, 2);
    cout << dynamic_cast<RealValue*>(x[0])->GetValue() << " " << dynamic_cast<RealValue*>(x[1])->GetValue() << endl << endl;
}

void TestBadCast(){
    cout << "TestBadCast" << endl;
    Value* matrix[4];
    Value* rhs[2];
    Value* x[2];
    matrix[0] = new RealValue(-3.0);
    matrix[1] = new RealValue(2.0);
    matrix[2] = new RealValue(-6.0);
    matrix[3] = new RealValue(-9.0);
    rhs[0] = new AdvancedValue(19.0);
    rhs[1] = new AdvancedValue(51.0);
    x[0] = new RealValue();
    x[1] = new RealValue();
    try{
        Solve(matrix, rhs, x, 2);
    }
    catch (bad_cast){
        cout << "Bad cast" << endl << endl;
    }
}

void TestDevideByZero(){
    cout << "TestDevideByZero" << endl;
    try{
        RealValue a(1);
        RealValue b(0);
        a /= b;
    }
    catch (domain_error){
        cout << "Devide by zero" << endl << endl;
    }
}

void LongTest(){
    cout << "LongTest" << endl;
    for(int size = 1; size <= 15; size++){
        cout << size << endl;
        Value* matrix[size * size];
        Value* rhs[size];
        Value* x[size];
        for(int i = 0; i < size; i++){
            double sum = 0.0;
            for(int j = 0; j < size; j++){
                matrix[i * size + j] = new AdvancedValue(1. / ((double)i + (double)j + 1.));
                if(j % 2 == 0)
                    sum += 1. / ((double)i + (double)j + 1.);
            }
            rhs[i] = new AdvancedValue(sum);
            x[i] = new AdvancedValue();
        }
        Solve(matrix, rhs, x, size);
        for(int i = 0; i < size; i++){
            cout << dynamic_cast<AdvancedValue*>(x[i])->GetValue() << ": " << dynamic_cast<AdvancedValue*>(x[i])->GetError() << " ";
        }
        cout << endl;
    }
}

int main() {
    TestOk();
    TestBadCast();
    TestDevideByZero();
    LongTest();

    return 0;
}
