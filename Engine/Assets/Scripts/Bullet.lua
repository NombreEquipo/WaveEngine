-- BULLET CONTROLLER - SIMPLE PROJECTILE

public = {
    speed = 20.0,
    lifetime = 5.0
}

local timeAlive = 0
local direction = {x = 0, y = 0, z = 1}  -- Default forward
local initialized = false
local hasMoved = false  -- Track if we've set initial position

function Start(self)
    Engine.Log("=== Bullet Spawned ===")
end

function Update(self, dt)
    -- Initialize on first Update (spawn data is ready now)
    if not initialized then
        local data = _G.nextBulletData
        if data and data.x and data.y and data.z then

            -- Clear the global data for next bullet
            _G.nextBulletData = nil

            -- Set initial position
            self.transform:SetPosition(data.x, data.y, data.z)

            -- Set rotation
            self.transform:SetRotation(-90, data.angle or 0, 0)

            -- Set scale
            local bulletScale = data.scale or 1.0
            self.transform:SetScale(bulletScale, bulletScale, bulletScale)

            -- Use pre-calculated direction from turret
            direction.x = data.dirX or 0
            direction.y = 0
            direction.z = data.dirZ or 1

            initialized = true

            -- Don't move this frame, just set position
            return
        else
            Engine.Log("[Bullet] WARNING: No spawn data - using defaults")
            initialized = true
        end
    end
    
    local pos = self.transform.position
    
    if pos == nil then
        Engine.Log("ERROR: Bullet position is nil")
        return
    end
    
    -- Get speed and lifetime from public variables
    local speed = self.public and self.public.speed or 20.0
    local lifetime = self.public and self.public.lifetime or 5.0
    
    -- Move bullet in calculated direction
    local newX = pos.x + direction.x * speed * dt
    local newY = pos.y + direction.y * speed * dt
    local newZ = pos.z + direction.z * speed * dt
    
    self.transform:SetPosition(newX, newY, newZ)
    
    -- Lifetime control
    timeAlive = timeAlive + dt
    
    if timeAlive >= lifetime then
        Engine.Log(string.format(" Bullet destroyed after %.2f seconds", timeAlive))
        self:Destroy()
    end
end