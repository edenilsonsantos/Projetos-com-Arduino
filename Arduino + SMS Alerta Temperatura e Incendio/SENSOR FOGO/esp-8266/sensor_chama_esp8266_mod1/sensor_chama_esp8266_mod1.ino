
-- Sensor de chama e fogo
FIRE_PIN = 7 -- GPIO 13

 
-- envia notificação para o celular.
function notify_pushbullet()
    http.post('https://api.pushbullet.com/v2/pushes',
              'Content-Type: application/jsonrnAccess-Token: TOKEN_DO_PUSHBULLETrn',
              '{"body":"O sensor detectou fogo, verifique.","title":"Fogo detectado!!!","type":"note"}',
    function (code, data)
        print(code)
    end)
 
-- inicia o processo de verificação de gás ou fogo
function start(){
    
        -- verifica se detectou chamas.
        if gpio.read(FIRE_PIN == 1 ){
            serial.print("Fogo detectado");
        }
}
 
-- termina o processo de verificação de gás ou fogo

function configure()
    -- configura os GPIO.
    gpio.mode(FIRE_PIN, gpio.INPUT)

 
    -- configura o ESP para conectar no Wifi.
    wifi.setmode(wifi.STATION)
    wifi.sta.config("SSID_WIFI", "SENHA_WIFI")
    wifi.sta.connect()
end
 
configure()
start()
