-- HUDController.lua
-- Manages all updates to the player's Heads-Up Display.

local STAMINA_BAR_MAX_HEIGHT = 56.0
local HEALTH_BAR_MAX_HEIGHT  = 74.0

-- No caching to ensure smooth updates and avoid "trompicones".

local function RefreshHealthBar(health)
    -- Clamp health to prevent visual errors
    local clampedHealth = math.max(0, math.min(100, health))
    UI.SetElementHeight("HealthGrid", (clampedHealth / 100.0) * HEALTH_BAR_MAX_HEIGHT)
end

local function RefreshStaminaBar(stamina)
    local clampedStamina = math.max(0, math.min(100, stamina))
    UI.SetElementHeight("StaminaGrid", (clampedStamina / 100.0) * STAMINA_BAR_MAX_HEIGHT)
end

local function RefreshPotionUI(potions)
    local clampedPotions = math.max(0, potions)
    UI.SetElementText("PotionsNumber", tostring(clampedPotions))
    UI.SetElementVisibility("Potion_Image",  clampedPotions > 0)
    UI.SetElementVisibility("PotionsNumber", clampedPotions > 0)
end

-- Force a full refresh of the HUD. Can be called from other scripts.
function ForceRefreshHUD()
    if _G.PlayerInstance and _G.PlayerInstance.public then
        local p = _G.PlayerInstance.public
        RefreshHealthBar(p.health)
        RefreshStaminaBar(p.stamina)
        RefreshPotionUI(p.potionCount)
    else
        -- If no player, set to a default state
        RefreshHealthBar(100)
        RefreshStaminaBar(100)
        RefreshPotionUI(2) -- Initial number of potions
    end
end
_G.ForceRefreshHUD = ForceRefreshHUD

function Start(self)
    -- Ensure the HUD is in a clean state on start
    ForceRefreshHUD()
end

function Update(self, dt)
    -- Continuously update the HUD based on the player's state
    if _G.PlayerInstance and _G.PlayerInstance.public then
        local p = _G.PlayerInstance.public
        RefreshHealthBar(p.health)
        RefreshStaminaBar(p.stamina)
        RefreshPotionUI(p.potionCount)
    end
end