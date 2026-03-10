local atan2 = math.atan
local pi    = math.pi
local sqrt  = math.sqrt

public = {
    moveSpeed = 10.0,
    rotationSpeed = 15.0
}

function Start(self)

    self.nav = self.gameObject:GetComponent("Navigation")
    self.rb  = self.gameObject:GetComponent("Rigidbody")
    self.currentY = 0

    if self.nav then
        self.nav:SetDestination(0, 0, 0)
    end
end

function Update(self, dt)
    if self.nav == nil or self.rb == nil then
        self.nav = self.gameObject:GetComponent("Navigation")
        self.rb  = self.gameObject:GetComponent("Rigidbody")
        self.currentY = 0
        if self.nav and self.transform then
            local pos = self.transform.position
            self.nav:SetDestination(pos.x, pos.y, pos.z)
        end
        return
    end

    -- Set destination on Space key press
    if Input.GetKeyDown("Space") then
        local mX, mY = Input.GetMousePosition()
        if mX ~= nil then
            local wX, wZ = Camera.GetScreenToWorldPlane(mX, mY, 0)
            if wX ~= nil then
                Engine.Log("Destino: " .. wX .. ", 0, " .. wZ)
                self.nav:SetDestination(wX, 0, wZ)
            end
        end
    end

    -- Get movement direction
    local dx, dz = self.nav:GetMoveDirection(0.3) 
    local vx, vy, vz = self.rb:GetLinearVelocity()

    if dx == 0 and dz == 0 then
        self.rb:SetLinearVelocity(0, vy, 0)
    else
        -- Normalize direction vector
        local mag = sqrt(dx * dx + dz * dz)
        if mag > 0 then
            dx = dx / mag
            dz = dz / mag
        end

        -- Calculate Rotation
        local targetAngle = atan2(dx, dz) * (180.0 / pi)
        if self.currentY == nil then self.currentY = targetAngle end

        local diff = targetAngle - self.currentY
        if diff > 180 then diff = diff - 360 end
        if diff < -180 then diff = diff + 360 end

        -- Apply Rotation
        self.currentY = self.currentY + (diff * self.public.rotationSpeed * dt)

        if self.transform then
            self.transform:SetRotation(0, self.currentY, 0)
        end 

        -- Apply Movement       
        local speed = self.public.moveSpeed
        self.rb:SetLinearVelocity(dx * speed, vy, dz * speed)
    end
end
