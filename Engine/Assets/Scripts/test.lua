
x = 0.0;
function Start()
    obj  = FindGameObject("this")
    obj2 = FindGameObject("City_building_010")
    print("Hello from Lua Start")
end

function Update()

    SetPosition(obj,0, x, 0)
    SetRotation(obj,0, x, 0)
    SetPosition(obj2,0, x, 0)

    x = x + 0.1

end
