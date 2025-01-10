#ifndef EXITER_H
#define EXITER_H

class Exiter {
private:
    static Exiter instance;
    bool shouldClose;
    Exiter();
public:
    Exiter(const Exiter&) = delete;
    static Exiter& Get();

    bool ShouldCloseGame();

    void CloseGame();

    ~Exiter();
};

#endif