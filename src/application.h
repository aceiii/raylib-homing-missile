#ifndef __GAME_APPLICATION_H__
#define __GAME_APPLICATION_H__


class Application {
public:
    virtual ~Application() = default;

    int Run();
    int GetFrameTime() const;
    int GetFPS() const;
    
protected:
    int GetScreenWidth() const;
    int GetScreenHeight() const;

    virtual bool OnInit() = 0;
    virtual void OnCleanup() = 0;
    virtual void OnUpdate(float dt) = 0;
    virtual void OnRender() = 0;
    virtual bool OnProcessEvents() = 0; 

private:
    bool Init();
    void Shutdown();
    void MainLoop();
    void UpdateFPS(float dt);
};


#endif//__GAME_APPLICATION_H__