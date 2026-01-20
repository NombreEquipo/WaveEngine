
x = 0.0;
y = 0.0;
rot = 0.0;
speed = 0.005;
bullets = {}
angles = {}
DeleteTime = {}

function Start()
    obj = FindGameObject("this")
    turret = FindGameObject("TankTurret")
    if obj == nil then
        print("ERROR: No se ha encontrado el objeto!")
    else
        print("Objeto encontrado correctamente")
    end
    print("Hello from Lua Start")   
end

function Update()

    if Input.W then
        MoveForward(obj, x, speed)
    end

    if Input.S then
        MoveBackward(obj, x, speed)
    end

    if Input.D then
        SetRotation(obj,0, x, 0)
        x = x + 1
    end 
    if Input.A then 
        SetRotation(obj,0, x, 0)
        x = x - 1
    end
    if Input.MouseLeft  then
        print("CLick")
        print(Input.MouseX)
        print(Input.MouseY)
        print(angleDeg)

    end   

    if Input.MouseRight  and not lastMouseLeft  then    
        bullet = CreatePrimitive("Sphere", "bullet")
        table.insert(bullets, bullet)

        TempPos = GetPosition(obj)
        SetPosition(bullet,TempPos.x,0,TempPos.z)

        local tempA = GetRotation(turret)
        table.insert(angles, tempA.y)

        table.insert(DeleteTime, 1000)
    end   

    lastMouseLeft = Input.MouseRight            

    local dx = Input.MouseX  
    local dy = Input.MouseY * -1
    local angle  = atan2(dy,dx)
    angleDeg = math.deg(angle)
    SetRotation(turret,0,angleDeg,0)

    cnt = 1
    for _, bullet in ipairs(bullets) do
    shoot(bullet, angles[cnt],cnt)
    cnt = cnt +1
    end


end

function atan2(y, x)
    if x > 0 then
        return math.atan(y / x)
    elseif x < 0 then
        if y >= 0 then
            return math.atan(y / x) + math.pi
        else
            return math.atan(y / x) - math.pi
        end
    elseif y > 0 then
        return math.pi / 2
    elseif y < 0 then
        return -math.pi / 2
    else
        return 0
    end
end

function shoot(bullet, ang, counter)

    if DeleteTime[counter] <= 0 then
        DeleteGameObject(bullet)
        table.remove(bullets, counter)
        table.remove(angles, counter)
        table.remove(DeleteTime, counter)

    else
        local rad = math.rad(ang)
        local dirX = math.cos(rad)
        local dirZ = math.sin(rad)
        local pos = GetPosition(bullet)
        local speed = 0.05

        SetPosition(bullet, pos.x + dirX * speed, pos.y, pos.z + dirZ * speed)
        DeleteTime[counter] = DeleteTime[counter]-1
    end
end

function MoveForward(object, angl, speed)
    local rad = math.rad(angl)
    local dirX = math.sin(rad)
    local dirZ = math.cos(rad)
    local pos = GetPosition(object)

    SetPosition(object, pos.x + dirX * speed, pos.y, pos.z + dirZ * speed)
end

function MoveBackward(object, angl, speed)
    local rad = math.rad(angl)
    local dirX = math.sin(rad)
    local dirZ = math.cos(rad)
    local pos = GetPosition(obj)

    SetPosition(object, pos.x - dirX * speed, pos.y, pos.z - dirZ * speed)
end
