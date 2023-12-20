#include "World.h"

namespace Gloria
{
    World::World()
    {

    }

    World::~World()
    {

    }

    void World::Initialize()
    {

    }

    void World::Update(const GameTimer& gt)
    {

    }

    void World::EndFrame(const GameTimer& gt)
    {//TODO

    }

    void World::OnMouseDown(WPARAM btnState, int x, int y)
    {
        if ((btnState & MK_LBUTTON) != 0)
        {
            this->LastMousePostion.x = x;
            this->LastMousePostion.y = y;

            SetCapture(this->WindowHandle);
        }
    }

    void World::OnMouseUp(WPARAM btnState, int x, int y)
    {
        ReleaseCapture();
    }

    void World::OnMouseMove(WPARAM btnState, int x, int y)
    {
        if ((btnState & MK_LBUTTON) != 0)
        {
            float dx = 0.25f * static_cast<float>(x - this->LastMousePostion.x);
            float dy = 0.25f * static_cast<float>(y - this->LastMousePostion.y);

            // TODO : ÉãÏñ»ú
        }

        this->LastMousePostion.x = x;
        this->LastMousePostion.y = y;
    }

    void World::OnMouseWheel(float wheelDistance)
    {
        // TODO
    }

    void World::OnKeyboardInput(const GameTimer& gt)
    {
        const float dt = gt.DeltaTime();

        // TODO
    }
}