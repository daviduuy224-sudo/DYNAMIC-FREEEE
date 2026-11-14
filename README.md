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

- Si aparece `window-zero`, activa la asistencia y aumenta el margen. `time-budget` indica que la simulación agotó el tiempo; puedes aumentar **Prediction time budget**. `Objeto no compatible` muestra el ID y el tipo numérico del objeto que interrumpió la predicción; si activas **Diagnostic log**, estos datos aparecen como `object_id` y `object_type` en el registro. Comparte ambos números y el porcentaje del nivel al informar de un problema. `ring`, `dual`, `platformer`, `slope` y `force` también señalan situaciones que este prototipo no puede predecir con confianza. `no-physics-tick` indica que todavía no hay un tick válido.

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

- Intenta corregir pulsaciones y liberaciones tempranas en cubo, nave, OVNI, bola, wave, robot, araña y swing. Retarda una entrada cuando la trayectoria simulada parece más segura; no puede adelantar una pulsación tardía. La asistencia es experimental y no admite dual ni modo plataforma.

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

La simulación usa un jugador oculto y una ventana de predicción corta. No reproduce necesariamente el comportamiento de objetos móviles, triggers, portales ni de otros mods. En vehículos distintos de wave se deja pasar la entrada original si el jugador está en una pendiente, bajo una fuerza o si la trayectoria simulada toca una pendiente. Puede fallar o causar resultados inesperados; su precisión en niveles reales y en cada vehículo no está verificada. Esta versión no ofrece las demás funciones del mod privado Dynamic.

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
