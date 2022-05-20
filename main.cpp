#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "pipeline.h"
#include "camera.h"
#include "texture.h"
#include "lighting_technique.h"
#include "glut_backend.h"
#include "util.h"
#include "mesh.h"
#include "skybox.h"

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

using namespace std;

class Main : public ICallbacks
{
public:

    Main()
    {
        m_pLightingEffect = NULL;
        m_pGameCamera = NULL;
        m_pMesh = NULL;
        m_scale = 0.0f;
        m_pSkyBox = NULL;

        m_dirLight.AmbientIntensity = 0.2f;
        m_dirLight.DiffuseIntensity = 0.8f;
        m_dirLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        m_dirLight.Direction = Vector3f(1.0f, -1.0f, 0.0f);
    }

    virtual ~Main()
    {
        SAFE_DELETE(m_pLightingEffect);
        SAFE_DELETE(m_pSkyBox);
        SAFE_DELETE(m_pGameCamera);
        SAFE_DELETE(m_pMesh);
    }

    bool Init()
    {
        Vector3f Pos(3.0f, 8.0f, -10.0f);
        Vector3f Target(0.0f, -0.2f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);

        m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

        m_pLightingEffect = new LightingTechnique();

        if (!m_pLightingEffect->Init()) {
            printf("Error initializing the lighting technique\n");
            return false;
        }

        m_pLightingEffect->Enable();
        m_pLightingEffect->SetDirectionalLight(m_dirLight);
        m_pLightingEffect->SetTextureUnit(0);
        m_pLightingEffect->SetShadowMapTextureUnit(1);

        m_pMesh = new Mesh();

        if (!m_pMesh->LoadMesh("phoenix_ugv.md2")) {
            return false;
        }

        m_pSkyBox = new SkyBox(m_pGameCamera, m_persProjInfo);

        if (!m_pSkyBox->Init(".",
            "../Content/sp3right.jpg",
            "../Content/sp3left.jpg",
            "../Content/sp3top.jpg",
            "../Content/sp3bot.jpg",
            "../Content/sp3front.jpg",
            "../Content/sp3back.jpg")) {
            return false;
        }

        return true;
    }

    void Run()
    {
        GLUTBackendRun(this);
    }

    virtual void RenderSceneCB()
    {
        m_pGameCamera->OnRender();
        m_scale += 0.2f;

        RenderPass();

        glutSwapBuffers();
    }

    virtual void RenderPass()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_pLightingEffect->Enable();

        Pipeline p;
        p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);
        p.Scale(0.1f, 0.1f, 0.1f);
        p.WorldPos(0.0f, -5.0f, 3.0f);
        p.Rotate(0.0f, m_scale, 0.0f);
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());

        m_pLightingEffect->SetWVP(p.GetWVPTrans());
        m_pLightingEffect->SetWorldMatrix(p.GetWorldTrans());

        m_pMesh->Render();

        m_pSkyBox->Render();
    }

    virtual void IdleCB()
    {
        RenderSceneCB();
    }

    virtual void SpecialKeyboardCB(int Key, int x, int y)
    {
        m_pGameCamera->OnKeyboard(Key);
    }


    virtual void KeyboardCB(unsigned char Key, int x, int y)
    {
        switch (Key) {
        case 'q':
            glutLeaveMainLoop();
            break;
        }
    }


    virtual void PassiveMouseCB(int x, int y)
    {
        m_pGameCamera->OnMouse(x, y);
    }

private:

    LightingTechnique* m_pLightingEffect;
    Camera* m_pGameCamera;
    float m_scale;
    DirectionalLight m_dirLight;
    Mesh* m_pMesh;
    SkyBox* m_pSkyBox;
    PersProjInfo m_persProjInfo;
};


int main(int argc, char** argv)
{
    GLUTBackendInit(argc, argv);

    Magick::InitializeMagick(nullptr); 

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "Laba 4")) {
        return 1;
    }

    Main* pApp = new Main();

    if (!pApp->Init()) {
        return 1;
    }

    pApp->Run();

    delete pApp;

    return 0;
}