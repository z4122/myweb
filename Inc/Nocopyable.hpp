#pragma once

class Nocopyable{
protected:
    Nocopyable(){};
    ~Nocopyable(){};
private:
    Nocopyable(const Nocopyable&);
    const Nocopyable& operator = (const Nocopyable&);
};