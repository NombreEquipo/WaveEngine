public = {
    moveSpeed = 5.0
}

function Start(self)

    self.nav = self.gameObject:GetComponent("Navigation")
    self.rb  = self.gameObject:GetComponent("Rigidbody")

end

function Update(self, dt)
    if self.nav == nil or self.rb == nil then
        self.nav = self.gameObject:GetComponent("Navigation")
        self.rb  = self.gameObject:GetComponent("Rigidbody")
        return
    end

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

    local dx, dz = self.nav:GetMoveDirection(0.3) 
    local vx, vy, vz = self.rb:GetLinearVelocity()

    if dx == 0 and dz == 0 then
        self.rb:SetLinearVelocity(0, vy, 0)
    else
        local speed = self.public.moveSpeed
        self.rb:SetLinearVelocity(dx * speed, vy, dz * speed)
    end
end
