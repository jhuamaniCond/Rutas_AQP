import osmnx as ox
import matplotlib
matplotlib.use("TkAgg")  # Habilita ventana interactiva
import matplotlib.pyplot as plt
from matplotlib.widgets import Button
centro_arequipa = (-16.3989, -71.5350)  # (latitud, longitud)

# Descargar el grafo peatonal
G = ox.graph_from_point(centro_arequipa, dist=5000, network_type="walk", simplify=True)

# Dibujar el grafo sin cerrar la figura
fig, ax = ox.plot_graph(G, show=False, close=False)

# Estado de captura (inicialmente apagado)
capturando = {'activo': False}
nodos_seleccionados = []

# Función para manejar clics en el gráfico
def onclick(event):
    if not capturando['activo']:
        return
    if event.button != 1 or event.xdata is None or event.ydata is None:
        return

    nearest_node = ox.distance.nearest_nodes(G, X=event.xdata, Y=event.ydata)
    nodos_seleccionados.append(nearest_node)
    print(f"Nodo seleccionado: {nearest_node}")
    ax.plot(event.xdata, event.ydata, marker='o', color='red')
    fig.canvas.draw()

# Función para alternar el estado de captura
def toggle_capture(event):
    capturando['activo'] = not capturando['activo']
    estado = "ACTIVADO" if capturando['activo'] else "DESACTIVADO"
    print(f"Captura de clics: {estado}")
    boton.label.set_text("Desactivar" if capturando['activo'] else "Activar")
    fig.canvas.draw_idle()

# Crear botón
boton_ax = plt.axes([0.8, 0.01, 0.1, 0.05])  # (x, y, width, height) en porcentajes
boton = Button(boton_ax, "Activar")
boton.on_clicked(toggle_capture)

# Conectar evento de clic en la figura
fig.canvas.mpl_connect('button_press_event', onclick)

print("Presiona el botón para activar/desactivar la selección de nodos.")
plt.show()

# Guardar la ruta
with open("ruta_guardada.txt", "w") as f:
    for nodo in nodos_seleccionados:
        lat = G.nodes[nodo]['y']
        lon = G.nodes[nodo]['x']
        f.write(f"{lat},{lon}\n")

print("Ruta guardada como 'ruta_guardada.txt'")
