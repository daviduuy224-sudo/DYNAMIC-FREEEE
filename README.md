<!-- coderabbit-ai-generated:v1 -->

# Dynamic Assist (experimental)

Prototipo de asistencia de entrada para Geometry Dash 2.2081 en Windows con Geode 5.11.2. No se ha probado dentro del juego. No se puede garantizar que sea indetectable.

## Instalación

Coloca `dynamicassist.practice.geode` en la carpeta `Geometry Dash/geode/mods` y reinicia el juego. La versión del juego y de Geode debe coincidir con las indicadas arriba.

## Uso

<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:e6c194d601f543857de41fed979ec786d01c1acdd3e7e0c4810907afb1a8f097 -->

- Empieza desactivado. Los botones **ON/OFF**, **-** y **+** dentro del nivel activan o desactivan la asistencia y ajustan el margen de 0 a 16 ticks. F6 recorre esos valores; F7 alterna entre 0 y el último margen seleccionado (1 si aún no hay uno). También puedes cambiar el margen en los ajustes del mod de Geode.
<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:b2cdadc253c6f1e31fd363f0b3f342d78bb24a792106722c443d09c85b277bf6 -->

- La interfaz muestra los clics corregidos, los ticks realmente retrasados, el último retraso y el motivo del último resultado. Una entrada pendiente interrumpida se cuenta solo por los ticks que llegaron a transcurrir; las entradas descartadas no se cuentan. **Show in-level controls** permite ocultar los controles.
<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:e79d96579e974a355d365f16a8354abeb23543a6f24382e9604d68168fdf0b3d -->

- Si aparece `window-zero`, activa la asistencia y aumenta el margen. `time-budget` indica que la simulación agotó el tiempo; puedes aumentar **Prediction time budget**. <!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:099104d1848cfafa9b229b33d35764e8bed8915e39b3dda004e27c932054805e -->

`Interaccion no simulada` muestra el ID y el tipo numérico del objeto que interrumpió la predicción

<!-- /coderabbit-ai-generated:v1 -->; si activas **Diagnostic log**, estos datos aparecen como `object_id` y `object_type` en el registro. Comparte ambos números y el porcentaje del nivel al informar de un problema. <!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:6515b958e9fc606b12e4466db277514215ada4c97c133ddc87737e9a55299cb2 -->

`ring` señala un contacto que no pudo copiarse o un orbe con efectos de grupo. `dual` y `platformer` señalan modos que este prototipo todavía no puede predecir con confianza.

<!-- /coderabbit-ai-generated:v1 --> `no-physics-tick` indica que todavía no hay un tick válido.

<!-- /coderabbit-ai-generated:v1 -->

<!-- /coderabbit-ai-generated:v1 -->
<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:f98c5183c90792e8e75091694fb1bad7e7d268779ad6d207277e2cf7f46c4b47 -->

<!-- coderabbit-ai-generated-section:v1:81d26a16339d14f979dc1e595068d1b1ba6ccf68332d1b7dcd64f1af9880b803 -->

- **Prediction time budget** ajusta el tiempo máximo dedicado a cada entrada entre 1 y 33 ms (12 ms por defecto). Un presupuesto mayor permite evaluar más candidatos, pero puede reducir los FPS y no garantiza que la simulación termine. El impacto depende del equipo y del nivel.

<!-- /coderabbit-ai-generated:v1 -->

<!-- /coderabbit-ai-generated:v1 -->
<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:df0ef8ff5b1acdff7815a22d8ce23d3407b5dbf01a0a055f42258bcf196e87c8 -->

<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:f258328710015f692d8ebf1465a151908e2e73545f7fa67ea48850208fc4e5d0 -->

- Intenta corregir pulsaciones y liberaciones tempranas en cubo, nave, OVNI, bola, wave, robot, araña y swing. Retarda una entrada cuando la trayectoria simulada parece más segura; no puede adelantar una pulsación tardía. La asistencia es experimental y no admite dual ni modo plataforma.
<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:0922db1ff39fcb3ad49b8fe07f3ebc0849fa77ba978ccd273fa5418acce39bc7 -->

<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:53f00f2a509fa1504a21eca1aa38e31ec4c8429809854321cf5ab71edd2a9086 -->

<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:2712945a6783f147cc46d17d18a7aeb157e3c0ccae12646d44df9ee368a798cc -->

- Los sólidos, peligros y peligros animados se pasan al motor de colisiones por tipo, sin una lista de IDs. La versión 0.4.0 añade portales de los ocho vehículos, gravedad y tamaño; portales de velocidad 200, 201, 202, 203 y 1334; orbes amarillos, rosas, azules, verdes, rojos, negros, dash y araña, y pads amarillos, rosas, azules, rojos y de araña. Orbes y pads usan copias privadas y sus colisiones nativas. Las activaciones no se escriben sobre los objetos reales. Se requiere comprobar estas interacciones dentro del juego; una compilación correcta no demuestra precisión de física.

<!-- /coderabbit-ai-generated:v1 -->

<!-- /coderabbit-ai-generated:v1 -->

<!-- /coderabbit-ai-generated:v1 -->

<!-- /coderabbit-ai-generated:v1 -->

<!-- /coderabbit-ai-generated:v1 -->
<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:12823679ed97b1c88876fc869d71cc4e98738e078578f89d0f0d7060688fd0e9 -->

<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:ca05be8e88cdea43c0fddd1f04b29aedaa7d662ddecc896d5a1e363bf661807d -->

<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:1785ef27a800626443ffcf36388afdec9edfd39fa1adb96630aee389b051ed24 -->

- Si la predicción no resulta clara o agota su presupuesto, se envía la entrada original. Si llega otra entrada mientras hay una pendiente, esta se entrega inmediatamente antes de la nueva. Los retrasos previstos aparecen en el registro de Geode.
- Una entrada pendiente se entrega también si el tick de física no es válido o si se reduce el margen por debajo de su retraso previsto. F6 y F7 responden mientras la ventana del juego está en primer plano.

<!-- /coderabbit-ai-generated:v1 -->
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

<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:775bde7f1f5e696a97a062f5173891f8424ce6e9badf953cfb3b844d5074361f -->

La simulación usa un jugador oculto y una ventana corta. Cada candidato se restaura con un checkpoint nativo; <!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:be99b1b723d72b97ed9465769bf12a6d77ff1efdee25281d68489b3875414c48 -->

su contexto de portales y activaciones temporales se restaura al terminar

<!-- /coderabbit-ai-generated:v1 -->, incluso si se agota el presupuesto. El dash ya puede predecirse con una copia de su orbe. Este cambio amplía el comportamiento anterior, que interrumpía la predicción ante orbes, dash y portales de vehículo/velocidad.

<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:b5c68e884f09f98f0edc627d496f72a02e3b40485ea355f02b2f3a4c3c4ade84 -->

No se simula el futuro completo del nivel: los triggers, objetos móviles, efectos de otros mods, portales de teletransporte, dual y plataforma siguen requiriendo tratamiento adicional. Los orbes que activan grupos personalizados se consideran inciertos. Las pendientes y fuerzas que ya afectan al jugador usan ahora el estado del checkpoint y la física nativa; esta ampliación sustituye el bloqueo anterior fuera de wave. Los cambios de vehículo usan las funciones nativas del jugador y el contexto del portal, pero no se ha verificado dentro del juego la transición del techo/suelo de la nave ni los ajustes de cámara. Puede fallar o causar resultados inesperados. Esta versión no ofrece todas las funciones del mod privado Dynamic.

<!-- /coderabbit-ai-generated:v1 -->

<!-- /coderabbit-ai-generated:v1 -->

<!-- /coderabbit-ai-generated:v1 -->

<!-- /coderabbit-ai-generated:v1 -->

## Compilación

<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:144b70d23e249d66e696ca1eb8dd6716e6c6f4cd895dca82cfdff4c9217f5622 -->

Instala Geode SDK 5.11.2 y Geode CLI para Windows, define `GEODE_SDK` con la ruta del SDK y ejecuta `cmake -S . -B build` seguido de `cmake --build build --config Release`.

El flujo de GitHub Actions `Validate and build` ejecuta las pruebas y compila el mod para Windows. Cuando termine correctamente, descarga el artefacto `DynamicAssist-Windows` desde la ejecución. El archivo `DynamicAssist-v0.2.2.geode` incluido en el repositorio es la versión anterior y no contiene estos cambios.

<!-- /coderabbit-ai-generated:v1 -->

## Licencia

<!-- coderabbit-ai-generated:v1 -->

<!-- coderabbit-ai-generated-section:v1:9c7a3e50d3150b7bcb025efbeed0e34df7b1bbd4a00a534d0963f2f65f14b2a1 -->

GPL-3.0-only. La técnica de simulación con un jugador oculto se basa en [GD Mega Overlay](https://github.com/maxnut/GDMegaOverlay), de maxnu y SpaghettDev. Consulta `LICENSE`.

<!-- /coderabbit-ai-generated:v1 -->
