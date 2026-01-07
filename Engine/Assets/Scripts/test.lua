
x = 0.0;
function Start()
    obj = FindGameObject("this")
    if obj == nil then
        print("ERROR: No se ha encontrado el objeto!")
    else
        print("Objeto encontrado correctamente")
    end	

    print("Hello from Lua Start")
end

function Update()

    if Input.W then
        SetPosition(obj,0, x, 0)
        SetRotation(obj,0, x, 0)
        x = x + 0.1

    end

    if Input.S then
        SetPosition(obj,0, x, 0)
        SetRotation(obj,0, x, 0)
        x = x - 0.1

    end
    if Input.MouseLeft then
        print(Input.MouseX)
        print(Input.MouseY)

    end
        if Input.MouseRight then
        print(Input.MouseX)
        print(Input.MouseY)

    end



end
