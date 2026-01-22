-- TANK MOVEMENT CONTROLLER
-- Solo maneja el movimiento adelante/atrás
-- Lee la rotación de Base.001 para saber hacia dónde moverse

public = {
    moveSpeed = 5.0
}

local baseObject = nil

function Start(self)
    Engine.Log("=== Tank Movement Controller Started ===")
    Engine.Log("Controls:")
    Engine.Log("  W/S = Forward/Backward")
    
    -- Buscar el objeto Base para leer su rotación
    baseObject = GameObject.Find("Base.001")
    if baseObject then
        Engine.Log("[Tank] Base.001 found for reading rotation")
    else
        Engine.Log("[Tank] WARNING: Base.001 not found!")
    end
end

function Update(self, dt)
    local pos = self.transform.position
    
    if pos == nil then
        Engine.Log("ERROR: Position is nil")
        return
    end
    
    -- Buscar Base si no está inicializado
    if not baseObject then
        baseObject = GameObject.Find("Base.001")
    end
    
    -- Obtener velocidad
    local moveSpeed = self.public and self.public.moveSpeed or 5.0
    
    -- Leer la rotación de Base.001
    -- Como Base es hijo de Tank (que no rota), la rotación local de Base ES la rotación mundial
    local visualYaw = 0
    if baseObject and baseObject.transform and baseObject.transform.rotation then
        visualYaw = baseObject.transform.rotation.y or 0
        
        -- Log constante para debug
        Engine.Log(string.format("[Tank] Base Y rotation: %.2f | Moving direction: X=%.2f, Z=%.2f", 
            visualYaw, math.sin(math.rad(visualYaw)), math.cos(math.rad(visualYaw))))
    else
        Engine.Log("[Tank] ERROR: Cannot read Base rotation!")
    end
    
    -- Calcular vector forward basado en la rotación de Base
    local radians = math.rad(visualYaw)
    local forwardX = math.sin(radians)
    local forwardZ = math.cos(radians)
    
    -- Movimiento adelante/atrás (W/S)
    if Input.GetKey("S") then
        self.transform:SetPosition(
            pos.x - forwardX * moveSpeed * dt,
            pos.y,
            pos.z - forwardZ * moveSpeed * dt
        )
    end
    
    if Input.GetKey("W") then
        self.transform:SetPosition(
            pos.x + forwardX * moveSpeed * dt,
            pos.y,
            pos.z + forwardZ * moveSpeed * dt
        )
    end
end