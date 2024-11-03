require "packages.class"

-- Shorthand Variables
local sw = scarlet.window.width
local sh = scarlet.window.height
local scx = sw * 0.5
local scy = sh * 0.5

-- Other variables
local time = 0
local heydots = ""
local input_vec = {
  down = 0,
  left = 0,
  right = 0,
  up = 0,
}
local speed = 200

-- Gizmos
local bg = Quad.new()
local dummy = Sprite.new()
local goodboy = Quad.new()
local hey = Label.new()
local bass = Sound.new()

function init()
  bg.name = "BG"
  bg.color = {r = 0.1, g = 0.1, b = 0.1, a = 1}
  bg.x = scx
  bg.y = scy
  bg.w = sw
  bg.h = sh

  dummy:LoadTexture("assets/dummy.png")
  dummy.color = {r = 0, g = 1, b = 1, a = 1}
  dummy.x = scx
  dummy.y = scy

  goodboy.name = "GoodBoy"
  goodboy.color = {r = 0.5, g = 0, b = 1, a = 1}
  goodboy.x = scx
  goodboy.y = scy
  goodboy.w = 64
  goodboy.h = 64

  hey:LoadFont("assets/sudo.ttf", 48)
  hey.color = {r = 1, g = 1, b = 0, a = 1}
  hey.text = "Hey Jude"
  hey.align.h = 0
  hey.align.v = 1
  hey.x = 24
  hey.y = sh - 24

  bass:LoadSource("assets/bass.wav")
  bass.volume = 0.25

  scarlet.music.play("assets/snuffy.ogg")
  scarlet.music.volume(0.25)
end

function input(event)
  if event.type == "KeyDown" then
    local key = event.button:upper()
    if key == "ESCAPE" then scarlet.exit() end
    if key == "1" then bass:Play() end
    input_vec.up = key == "UP" and 1 or input_vec.up
    input_vec.down = key == "DOWN" and 1 or input_vec.down
    input_vec.left = key == "LEFT" and 1 or input_vec.left
    input_vec.right = key == "RIGHT" and 1 or input_vec.right
  elseif event.type == "KeyUp" then
    local key = event.button:upper()
    input_vec.up = key == "UP" and 0 or input_vec.up
    input_vec.down = key == "DOWN" and 0 or input_vec.down
    input_vec.left = key == "LEFT" and 0 or input_vec.left
    input_vec.right = key == "RIGHT" and 0 or input_vec.right
  end
end

function update(dt)
  goodboy.rot = goodboy.rot + 180 * dt
  dummy.rot = dummy.rot - 90 * dt

  hey.aux = hey.aux + dt
  if hey.aux >= 0.5 then
    heydots = heydots.."."
    if heydots == "...." then
      heydots = ""
    end
    hey.text = "Hey Jude"..heydots
    hey.aux = 0
  end

  goodboy.x = goodboy.x + (speed * dt) * (input_vec.right - input_vec.left)
  goodboy.y = goodboy.y + (speed * dt) * (input_vec.down - input_vec.up)

  if goodboy.x > sw + goodboy.w then goodboy.x = -goodboy.w
  elseif goodboy.x < -goodboy.w then goodboy.x = sw + goodboy.w end
  if goodboy.y > sh + goodboy.h then goodboy.y = -goodboy.h
  elseif goodboy.y < -goodboy.h then goodboy.y = sh + goodboy.h end

  bass.x = goodboy.x
  bass.y = goodboy.y
  bass:SetPosition()

  time = time + dt
  if time >= 5 then
    --scarlet.exit()
  end
end

function draw()
  bg:Draw()
  dummy:Draw()
  goodboy:Draw()
  hey:Draw()
end
