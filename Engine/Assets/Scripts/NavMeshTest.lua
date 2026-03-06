local nav = nil
local rb  = nil

local MOVE_SPEED        = 5.0
local ARRIVAL_THRESHOLD = 0.3

function Start(self)
    nav = self:GetComponent("Navigation")
    rb  = self:GetComponent("Rigidbody")

    if nav == nil then Engine.Log("ERROR: Navigation NO encontrado") end
    if rb  == nil then Engine.Log("ERROR: Rigidbody NO encontrado")  end
end

function Update(self, dt)

    -- Input: Space + click para mover
    if Input.GetKeyDown("Space") then
        local mX, mY = Input.GetMousePosition()
        if mX ~= nil then
            local wX, wZ = Camera.GetScreenToWorldPlane(mX, mY, 0)
            if wX ~= nil then
                Engine.Log("Destino: " .. wX .. ", 0, " .. wZ)
                Navigation.SetDestination(nav, wX, 0, wZ)
            end
        end
    end

    if nav == nil or rb == nil then return end

    -- GetMoveDirection lee el Transform internamente en C++,
    -- avanza los waypoints y devuelve la dirección normalizada
    local dx, dz = Navigation.GetMoveDirection(nav, ARRIVAL_THRESHOLD)

    -- Conserva la velocidad Y de PhysX (gravedad)
    local _, vy, _ = Rigidbody.GetLinearVelocity(rb)

    if dx == 0 and dz == 0 then
        -- Sin destino o llegó: frenar en X/Z
        Rigidbody.SetLinearVelocity(rb, 0, vy, 0)
    else
        Rigidbody.SetLinearVelocity(rb, dx * MOVE_SPEED, vy, dz * MOVE_SPEED)
    end
end