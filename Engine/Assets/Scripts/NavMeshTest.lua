local nav = nil

function Start(self)
    nav = self:GetComponent("Navigation")
end

function Update(self, dt)
    nav:Update(dt)  -- esto es lo que realmente mueve el agente cada frame

    local mouseX, mouseY = Input.GetMousePosition()
    if mouseX ~= nil and Input.GetKeyDown("Space") then
        local worldX, worldZ = Camera.GetScreenToWorldPlane(mouseX, mouseY, 0)
        nav:SetDestination(worldX, 0, worldZ)
    end
end