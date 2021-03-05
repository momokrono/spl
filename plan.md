# sgl
Simple Graphics Library

## IMAGE

Disegna solo "cose facili"
Metodo `draw`: richiede `drawable`

_fatto_

## COLLECTION

inheritance
    Pro:
        - insieme aperto
    con:
        - allocato dinamicamente
        - costo dell'indirection
        - costo della chiamata virtuale (final potrebbe ammortizzarlo)

variant
    pro:
        - allocato su stack
        - estensibile con una primitiva `dynamic_object` o qualcosa così
    con:
        - insieme chiuso
```cpp
variant<circle, line, rectangle, dynamic_object> // con i tipi drawable

template <typename T>
    requires drawable<T>
void image::draw(T roba)
{
    ...
}


spl::plotter::produce  genera una spl::graphics::image
spl::plotter::dump_on  scrive su una immagine esistente

plotter.set_axis(true);
```

L'immagine ha dei metodi per disegnare primitive
Ogni volta che si disegna, è l'immagine che la disegna
Il renderer accumula tutto e poi dumpa sull'immagine (ma è l'mmagine a disegnare)
Il plotter prende dei dati "grezzi" e li trasforma in comandi per il renderer

Il punto più basso diventa il layer e non l'immagine
È l'immagine che deve sapere come applicare i layer, esatto
Proprio come un renderer sa come gestire un'immagi no no perchè a quel punto il renderer lavora sui layer
E `image` diventa un container
Non proprio, image diventa una view su layer che è la view sui raw data di ogni singolo layer
Si

```cpp
image<managed_allocator<rgba>> img;
image img2;
```



----------------------------------------------------------------------------------------------------
## Cos'è IMAGE
    L'unità fondamentale di spl, possiede la raw memory con i colori, permette di salvare e caricare
    e di fare operazioni di modifica basilari.
## Cosa manca a IMAGE
    - draw
## Note
    using sf::Image = const splg::image;
    using sf::Canvas = splg::image;

----------------------------------------------------------------------------------------------------
## Cos'è un DRAWABLE
    È un concept che indica un oggetto disegnabile su un'image, deve implementare un metodo di segnatura
    `void render_on(image &)`
    `void operator()(image &)`

void image::draw(drawable auto f)
void image::draw(callable<splg::image> auto f)

----------------------------------------------------------------------------------------------------
## Cos'è RENDERER
    È quell'oggetto che contiene le istruzioni per disegnare su un'immagine ciò che voglio disegnare
    Ha diversi metodi che accumulano primitive in un buffer, e li applica tutti in una volta
## Cosa manca a RENDERER
    - (tutto)  :(
    - metodi per il chaining
    - definizione di drawable (variant? tipo polimorfico? un variant con tipo polimorfico?)
      (es: variant<circle, line, rectangle, dynamic_object> // con i tipi drawable)

```cpp
struct lambda_adaptor
{
    std::function<void(splg::image &)> f;
    void render_on(splg::image & img) { f(img); }
};
```
----------------------------------------------------------------------------------------------------
## Cos'è PLOTTER
    Il plotter accumula "dati grezzi" (coppie di punti, funzioni, roba così) e li processa in modo
    da ottenere delle primitive che il renderer (creato al volo quando necessario e subito distrutto)
    possa disegnare su un'image
## Cosa manca a PLOTTER
    - (tutto)  :(
----------------------------------------------------------------------------------------------------


```cpp
ren.from(0, 0)
   .line_to(19, 4)
   .color(green)
   .line_to(8, 14)
   ...
   ;

ren << line{0, 0, {0, 0}} << circle{10, {10, 4}} << color::blue << ...
ren << point{0, 0} << line_to(5, 3)

```
