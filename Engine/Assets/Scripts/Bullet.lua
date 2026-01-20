-- BULLET CONTROLLER - SIMPLE PROJECTILE

public = {
    speed = 20.0,
    lifetime = 5.0
}

local timeAlive = 0
local direction = {x = 0, y = 0, z = 0}
local initialized = false

function Start(self)
    Engine.Log("=== Bullet Spawned ===")
    
    local rot = self.transform.rotation
    
    if rot == nil then
        Engine.Log("ERROR: Bullet rotation is nil")
        return
    end
    
    local radians = math.rad(rot.y)
    
    -- Forward vector en el plano XZ
    direction.x = math.sin(radians)
    direction.y = 0  -- Sin componente vertical
    direction.z = math.cos(radians)
    
    -- Normalizar (debería estar normalizado ya, pero por seguridad)
    local length = math.sqrt(direction.x * direction.x + direction.z * direction.z)
    if length > 0 then
        direction.x = direction.x / length
        direction.z = direction.z / length
    end
    
    initialized = true
    
    Engine.Log(string.format("Bullet initialized | Angle: %.1f° | Direction: (%.2f, 0, %.2f)", 
        rot.y, direction.x, direction.z))
end

function Update(self, dt)
    if not initialized then
        return
    end
    
    local pos = self.transform.position
    
    if pos == nil then
        Engine.Log("ERROR: Bullet position is nil")
        return
    end
    
    -- Obtener velocidad desde variables públicas
    local speed = self.public and self.public.speed or 20.0
    local lifetime = self.public and self.public.lifetime or 5.0
    
    -- Mover la bala en la dirección calculada
    local newX = pos.x + direction.x * speed * dt
    local newY = pos.y + direction.y * speed * dt
    local newZ = pos.z + direction.z * speed * dt
    
    self.transform:SetPosition(newX, newY, newZ)
    
    -- Control de vida
    timeAlive = timeAlive + dt
    
    if timeAlive >= lifetime then
        Engine.Log("Bullet destroyed after " .. string.format("%.2f", timeAlive) .. " seconds")
        self:Destroy()
    end
end