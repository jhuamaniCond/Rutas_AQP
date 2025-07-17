import osmnx as ox

# Coordenadas aproximadas del centro de Arequipa
centro_arequipa = (-16.3989, -71.5350)  # (latitud, longitud)

# Descargar grafo peatonal con un radio mayor (por ejemplo, 5000 metros = 5 km)
G = ox.graph_from_point(centro_arequipa, dist=5000, network_type="walk", simplify=True)

# Guardar a archivo .graphml
ox.save_graphml(G, filepath="arequipa_expanded.graphml")