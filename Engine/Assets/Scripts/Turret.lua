-- TURRET CONTROLLER - MOUSE AIMING (HORIZONTAL ONLY)

public = {
    fireRate = 0.5,
    barrelOffset = 3.0,
    groundPlaneY = 0.0,
    bulletPrefab = "Bullet.prefab",
    pitchOffset = -90.0
}

local fireCooldown = 0

function Start(self)
    Engine.Log("=== Turret Controller Started ===")
    Engine.Log("Turret: " .. self.gameObject.name)
    Engine.Log("Controls: MOUSE = Aim | SPACE = Shoot")
    
    -- Asegurar que la torreta empiece en orientación correcta
    local pitchOffset = self.public and self.public.pitchOffset or 0.0
    self.transform:SetRotation(pitchOffset, 0, 0)
end

function Update(self, dt)
    -- === ROTACIÓN CON RATÓN (SOLO HORIZONTAL) ===
    local mouseX, mouseY = Input.GetMousePosition()
    
    if mouseX == nil or mouseY == nil then
        return
    end
    
    local groundPlaneY = self.public and self.public.groundPlaneY or 0.0
    local worldX, worldZ = Camera.GetScreenToWorldPlane(mouseX, mouseY, groundPlaneY)
    
    if worldX == nil or worldZ == nil then
        return
    end
    
    local pos = self.transform.position
    
    if pos == nil then
        Engine.Log("ERROR: self.transform.position is nil")
        return
    end
    
    -- Calcular dirección hacia el cursor
    local dx = worldX - pos.x
    local dz = worldZ - pos.z
    
    -- Verificar que hay movimiento significativo
    local distance = math.sqrt(dx * dx + dz * dz)
    if distance < 0.01 then
        return
    end
    
    -- Calcular ángulo usando atan2
    local angleRadians = math.atan(dx, dz)
    local angleDegrees = angleRadians * (180 / math.pi)
    
    -- Aplicar rotación con offset
    local pitchOffset = self.public and self.public.pitchOffset or 0.0
    self.transform:SetRotation(pitchOffset, angleDegrees, 0)
    
    -- === SISTEMA DE DISPARO ===
    fireCooldown = fireCooldown - dt
    
    if Input.GetKeyDown("Space") and fireCooldown <= 0 then
        FireBullet(self)
        fireCooldown = self.public and self.public.fireRate or 0.5
        Engine.Log("pium")
    end
end

function FireBullet(self)
    local bulletPrefab = self.public and self.public.bulletPrefab or "Bullet.prefab"
    local bullet = Prefab.Instantiate(bulletPrefab)
    
    if bullet == nil then
        Engine.Log("ERROR: Failed to instantiate bullet prefab: " .. bulletPrefab)
        return
    end
    
    local turretPos = self.transform.position
    local turretRot = self.transform.rotation
    
    if turretPos == nil or turretRot == nil then
        Engine.Log("ERROR: Cannot get turret transform data")
        return
    end
    
    -- Ignoramos pitchOffset porque es solo visual del modelo
    local radians = math.rad(turretRot.y)
    local forwardX = math.sin(radians)
    local forwardZ = math.cos(radians)
    
    -- Posición de spawn del proyectil
    local barrelOffset = self.public and self.public.barrelOffset or 3.0
    local spawnX = turretPos.x + forwardX * barrelOffset
    local spawnY = turretPos.y + 0.8
    local spawnZ = turretPos.z + forwardZ * barrelOffset
    
    bullet.transform:SetPosition(spawnX, spawnY, spawnZ)
    
    -- La bala no necesita el pitchOffset visual
    bullet.transform:SetRotation(0, turretRot.y, 0)
    
    Engine.Log(string.format(" Bullet fired - Angle: %.1f° | Dir: (%.2f, %.2f)", 
        turretRot.y, forwardX, forwardZ))
end