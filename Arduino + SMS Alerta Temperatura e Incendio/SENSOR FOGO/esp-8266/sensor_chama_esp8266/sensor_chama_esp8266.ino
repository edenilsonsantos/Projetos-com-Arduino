-- Buzzer
BUZ_PIN  = 6 -- GPIO 12
-- Sensor de chama e fogo
FIRE_PIN = 7 -- GPIO 13
-- Sensor de Gás
GAS_PIN  = 5 -- GPIO 14
 
BUZZ_TMR = 1
FIRE_GAS_TMR = 2
 
 
-- liga o alarme que irá fazer um beep a cada 1 segundo.
function buz_on()
    is_buz_on = false
 
    tmr.alarm(BUZZ_TMR, 1000, tmr.ALARM_AUTO, function()
        if is_buz_on then
            gpio.write(BUZ_PIN, gpio.LOW) -- on
        else
            gpio.write(BUZ_PIN, gpio.HIGH) -- off
        end
 
        is_buz_on = not is_buz_on
    end)
end
 
-- desliga o alarme
function buz_off()
    gpio.write(BUZ_PIN, gpio.HIGH)
    tmr.unregister(BUZZ_TMR)
end
 
-- verifica se o alarme já está ligado
function is_buz_enable()
    running, mode = tmr.state(BUZZ_TMR)
    return running
end
 
-- envia notificação para o celular.
function notify_pushbullet()
    http.post('https://api.pushbullet.com/v2/pushes',
              'Content-Type: application/jsonrnAccess-Token: TOKEN_DO_PUSHBULLETrn',
              '{"body":"O sensor detectou fogo ou gás, verifique.","title":"Fogo detectado!!!","type":"note"}',
    function (code, data)
        print(code)
    end)
end
 
-- inicia o processo de verificação de gás ou fogo
function start()
    buz_off()
 
    tmr.alarm(FIRE_GAS_TMR, 500, tmr.ALARM_AUTO, function()
        turn_buz_on = false
 
        -- verifica se detectou chamas.
        if gpio.read(FIRE_PIN) == 1 then
            turn_buz_on = true
        end
 
        -- verifica se detectou gás.
        if gpio.read(GAS_PIN) == 1 then
            turn_buz_on = true
        end
 
        if turn_buz_on then
            if not is_buz_enable() then
                buz_on()
                notify_pushbullet()
            end
        else
            buz_off()
        end
    end)
end
 
-- termina o processo de verificação de gás ou fogo
function stop()
    buz_off()
    tmr.unregister(FIRE_GAS_TMR)
end
 
function configure()
    -- configura os GPIO.
    gpio.mode(BUZ_PIN, gpio.OUTPUT)
    gpio.mode(FIRE_PIN, gpio.INPUT)
    gpio.mode(GAS_PIN, gpio.INPUT)
 
    -- configura o ESP para conectar no Wifi.
    wifi.setmode(wifi.STATION)
    wifi.sta.config("SSID_WIFI", "SENHA_WIFI")
    wifi.sta.connect()
end
 
configure()
start()