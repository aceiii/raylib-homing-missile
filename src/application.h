#ifndef __GAME_APPLICATION_H__
#define __GAME_APPLICATION_H__


class Application {
public:
    virtual ~Application() = default;

    int Run();
    int GetFrameTime() const;
    int GetFPS() const;
    
protected:
    bool Init();
    void Shutdown();
    void MainLoop();

    virtual bool OnInit() = 0;
    virtual void OnCleanup() = 0;
    virtual void OnUpdate(float dt) = 0;
    virtual void OnRender() = 0;
    virtual bool OnProcessEvents() = 0; 

private:
    void UpdateFPS(float dt);
};


#endif//__GAME_APPLICATION_H__