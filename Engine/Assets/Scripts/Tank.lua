-- TANK CONTROLLER - MOVEMENT ONLY
-- Controles:
-- W/S: Mover adelante/atrás
-- A/D: Mover izquierda/derecha (strafe)
-- Q/E: Rotar el tanque (cuerpo)
-- ==========================================

public = {
    moveSpeed = 5.0,
    strafeSpeed = 3.0,
    rotationSpeed = 90.0
}

function Start(self)
    Engine.Log("=== Tank Movement Controller Started ===")
    Engine.Log("Tank: " .. self.gameObject.name)
    Engine.Log("Controls:")
    Engine.Log("  W/S = Forward/Backward")
    Engine.Log("  A/D = Strafe Left/Right")
    Engine.Log("  Q/E = Rotate tank body")
end

function Update(self, dt)
    local pos = self.transform.position
    local rot = self.transform.rotation
    
    if pos == nil or rot == nil then
        Engine.Log("ERROR: Transform data is nil")
        return
    end
    
    -- Obtener velocidades desde variables públicas
    local moveSpeed = self.public and self.public.moveSpeed or 5.0
    local strafeSpeed = self.public and self.public.strafeSpeed or 3.0
    local rotationSpeed = self.public and self.public.rotationSpeed or 90.0
    
    -- Calcular vectores de dirección del tanque
    local tankRadians = math.rad(rot.y)
    local forwardX = math.sin(tankRadians)
    local forwardZ = math.cos(tankRadians)
    local rightX = math.cos(tankRadians)
    local rightZ = -math.sin(tankRadians)
    
    -- Movimiento adelante/atrás (W/S)
    if Input.GetKey("W") then
        self.transform:SetPosition(
            pos.x + forwardX * moveSpeed * dt,
            pos.y,
            pos.z + forwardZ * moveSpeed * dt
        )
    end
    
    if Input.GetKey("S") then
        self.transform:SetPosition(
            pos.x - forwardX * moveSpeed * dt,
            pos.y,
            pos.z - forwardZ * moveSpeed * dt
        )
    end
    
    -- Movimiento lateral (A/D) - Strafe
    if Input.GetKey("A") then
        self.transform:SetPosition(
            pos.x - rightX * strafeSpeed * dt,
            pos.y,
            pos.z - rightZ * strafeSpeed * dt
        )
    end
    
    if Input.GetKey("D") then
        self.transform:SetPosition(
            pos.x + rightX * strafeSpeed * dt,
            pos.y,
            pos.z + rightZ * strafeSpeed * dt
        )
    end
    
    -- Rotación del tanque (Q/E)
    if Input.GetKey("Q") then
        self.transform:SetRotation(rot.x, rot.y - rotationSpeed * dt, rot.z)
    end
    
    if Input.GetKey("E") then
        self.transform:SetRotation(rot.x, rot.y + rotationSpeed * dt, rot.z)
    end
end