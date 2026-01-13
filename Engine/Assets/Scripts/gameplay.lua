-- VARIABLES GLOBALES
myGameObject = nil
timer = 0
created = false

-- 1. FUNCION START (Se llama una vez al inicio)
function Start()
    -- Obtenemos el objeto que tiene este script (usando tu logica de "this")
    myGameObject = FindGameObject("this")
    
    -- Requisito: Log Hello World
    Log("Hello World - Iniciando Script!")
end

-- 2. FUNCION UPDATE (Se llama cada frame desde C++)
function Update()
    -- Requisito: Loguear periódicamente (para no saturar la consola)
    timer = timer + 1
    if timer > 600 then -- Asumiendo 60fps, cada 10 seg aprox
        Log("Hello World desde el Update (Keep Alive)")
        timer = 0
    end

    -- Requisito: Mover el GameObject
    -- Vamos a moverlo basándonos en el tiempo o input
    -- Como tu GetPosition devuelve una tabla, la usamos:
    local currentPos = GetPosition(myGameObject)
    
    -- Movemos el objeto en X automáticamente (efecto ping-pong o continuo)
    local newX = currentPos.x + 0.01
    
    -- Si tu Input W está presionado, movemos más rápido
    if Input.W then
        newX = newX + 0.1
        Log("Presionando W: Acelerando")
    end

    -- Llamamos a tu función de C++
    SetPosition(myGameObject, newX, currentPos.y, currentPos.z)

    -- Requisito: Pulsar tecla e Instanciar GameObject
    -- Usamos tu tabla Input que llenas en PushInput()
    -- Nota: En tu C++ usas KEY_REPEAT para W,A,S,D. 
    -- Para instanciar es mejor KEY_DOWN (una sola vez), 
    -- pero como en tu C++ usaste KEY_REPEAT para todo, añadiremos un flag 'created' 
    -- para que no cree 60 cubos por segundo.
    
    if Input.MouseLeft and created == false then
        -- Llamamos a tu función C++
        local newObj = CreatePrimitive("Cube", "CuboCreadoPorLua")
        
        -- Opcional: Mover el nuevo objeto para que no salga encima
        SetPosition(newObj, 0, 5, 0)
        
        Log("Nuevo Objeto Creado!")
        created = true -- Bloqueo simple para no crear infinitos
    end

    -- Reseteamos el flag si soltamos el click (opcional, logica simple)
    if not Input.MouseLeft then
        created = false
    end
end