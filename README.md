<!-- coderabbit-ai-generated:v1 -->

# Dynamic Assist (experimental)

Prototipo de asistencia de entrada para Geometry Dash 2.2081 en Windows con Geode 5.11.2. No se ha probado dentro del juego. No se puede garantizar que sea indetectable.

## Instalación

Coloca `dynamicassist.practice.geode` en la carpeta `Geometry Dash/geode/mods` y reinicia el juego. La versión del juego y de Geode debe coincidir con las indicadas arriba.

## Uso

- Empieza desactivado. Pulsa F6 dentro de un nivel para recorrer los márgenes de 0 a 8 ticks; 0 desactiva la asistencia. También se puede configurar en los ajustes del mod de Geode.
<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:df0ef8ff5b1acdff7815a22d8ce23d3407b5dbf01a0a055f42258bcf196e87c8 -->

- Intenta corregir pulsaciones y liberaciones tempranas en cubo, nave, OVNI, bola, wave, robot, araña y swing. Retarda una entrada cuando la trayectoria simulada parece más segura; no puede adelantar una pulsación tardía. La asistencia es experimental y no admite dual ni modo plataforma.

<!-- /coderabbit-ai-generated:v1 -->
<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:12823679ed97b1c88876fc869d71cc4e98738e078578f89d0f0d7060688fd0e9 -->

<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:ca05be8e88cdea43c0fddd1f04b29aedaa7d662ddecc896d5a1e363bf661807d -->

- Si la predicción no resulta clara, se envía la entrada original. Si llega otra entrada mientras hay una pendiente, esta se entrega inmediatamente antes de la nueva. Los retrasos previstos aparecen en el registro de Geode.
<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:1458e99181d8ee00db5c592757573eb254e81045f9adbdc82035e2cd0ce64da7 -->

- Para investigar por qué no se corrige un clic, activa **Diagnostic log** en los ajustes del mod y reproduce el problema. El registro local más reciente está en `geode/logs`; busca las líneas `Dynamic Assist diagnostic`. El registro indica también si una entrada pendiente fue reenviada o descartada. Desactiva el diagnóstico cuando termines.

<!-- /coderabbit-ai-generated:v1 -->

<!-- /coderabbit-ai-generated:v1 -->

<!-- /coderabbit-ai-generated:v1 -->

## Limitaciones

<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:11de41aa65a875ec4516b7917be272797c74020e1ce8eaf098e5928e4e9135a5 -->

<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:21a0002b3666c0616ea719d684b3442af1a14ad4c1e6939a2da4204801993050 -->

La simulación usa un jugador oculto y una ventana de predicción corta. No reproduce necesariamente el comportamiento de objetos móviles, triggers, portales ni de otros mods. En vehículos distintos de wave se deja pasar la entrada original si el jugador está en una pendiente, bajo una fuerza o si la trayectoria simulada toca una pendiente. Puede fallar o causar resultados inesperados; su precisión en niveles reales y en cada vehículo no está verificada. Esta versión no ofrece las demás funciones del mod privado Dynamic.

<!-- /coderabbit-ai-generated:v1 -->

<!-- /coderabbit-ai-generated:v1 -->

## Compilación

Instala Geode SDK 5.11.2 y Geode CLI para Windows, define `GEODE_SDK` con la ruta del SDK y ejecuta `cmake -S . -B build` seguido de `cmake --build build --config Release`.

## Licencia

<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:9c7a3e50d3150b7bcb025efbeed0e34df7b1bbd4a00a534d0963f2f65f14b2a1 -->

GPL-3.0-only. La técnica de simulación con un jugador oculto se basa en [GD Mega Overlay](https://github.com/maxnut/GDMegaOverlay), de maxnu y SpaghettDev. Consulta `LICENSE`.

<!-- /coderabbit-ai-generated:v1 -->
