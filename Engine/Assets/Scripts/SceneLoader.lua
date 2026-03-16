-- SceneLoader.lua

public = {
    temp1 = { type = "Scene", value = "" },
    temp2 = { type = "Scene", value = "" }
}

local assetsPath = Engine.GetAssetsPath()


function Update(self, dt)
    if Input.GetKeyDown("1") then
        Engine.LoadScene(assetsPath, self.public.temp1)
    
    end
    if Input.GetKeyDown("2") then
        Engine.LoadScene(assetsPath, self.public.temp2)
    end
end


