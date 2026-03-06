-- SceneLoader.lua

local scenesPath = Engine.GetScenesPath()

function Update(self, dt)
    if Input.GetKeyDown("1") then
        Engine.LoadScene(scenesPath .. "/Level1.json")
    end
end

