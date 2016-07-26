1.  DIRECTX 10 NOTLARIM

-   Pencere Çizimi için WinApi gerekli.

1.  WINAPI

    Every Windows program includes an entry-point function that is named either WinMain or wWinMain. Here is the signature for wWinMain.

-   wWinMain fonksiyonu main fonksiyonu olarak görev yapıyor.

    int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);

<!-- -->

-   **hInstance** is something called a "handle to an instance" or "handle to a module." The operating system uses this value to identify the executable (EXE) when it is loaded in memory. The instance handle is needed for certain Windows functions — for example, to load icons or bitmaps.

-   **hPrevInstance** has no meaning. It was used in 16-bit Windows, but is now always zero.

-   **pCmdLine** contains the command-line arguments as a Unicode string.

-   **nCmdShow** is a flag that says whether the main application window will be minimized, maximized, or shown normally.

<!-- -->

-   Calling Convention: Fonksiyona parametrelerin geçirilmesi, stack(yığın) temizliğini hangi fonksiyonun yapacağını ve hiç görmeyeceğiniz birkaç şeyi daha etkiler. “Calling convention” olarak WINAPI bildiricisine sahip olan bir fonksiyon parametreleri sağdan-sola değil de soldan-sağa geçirilir.

1.  WNDCLASSEX 

    Pencereyle ilgili tüm bilgiyi(kullanacağı simge veya varsa menüsü gibi) içerecek. Yaptığınız her Windows programında ihtiyacınıza uygun bir pencere sınıfı oluşturmalısınız. Bunun için **WNDCLASSEX** isimli yapının elamanlarını doldurmalısınız. "EX" eki "extended"ın kısaltması, WNDCLASS isimli eski bir yapı daha var biz yenisini kullanacağız. İşte WNDCLASSEX yapısı:

    typedef struct \_WNDCLASSEX {

        UINT    cbSize;

        UINT    style;

        WNDPROC lpfnWndProc;

        int     cbClsExtra;

        int     cbWndExtra;

        HANDLE  hInstance;

        HICON   hIcon;

        HCURSOR hCursor;

        HBRUSH  hbrBackground;

        LPCTSTR lpszMenuName;

        LPCTSTR lpszClassName;

        HICON   hIconSm;

    } WNDCLASSEX;

    **UINT cbSize:** Bu bayt olarak yapının boyutu. DirectX ile ilgilenirseniz bununla sık karşılaşacaksınız. Bu tip bir yapı(veya bu türden bir yapıya işaretçi), bir fonksiyona geçirilirken boyutun tekrar tekrar hesaplanması yerine buradan bakılabilir. Her zaman sizeof(WNDCLASSEX) olarak belirleyin.

    **UINT style:** Pencere tipi. CS\_ ile başlayan sabitleri alır.  | (bitwise OR,bit düzeyinde veya) operatorü ile birleştirerek birden fazla sabiti kullanabilirsiniz. Genellikle dört tane kullanacaksınız. Yazıyı kısa tutmak için sadece bunları açıklayacağım. MSDN Yardım’da tamamı detaylı olarak anlatılıyor. Visual C++ aldınız değil mi ?

    **CS\_HREDRAW** Pencere yatay olarak boyut değiştirdiğinde pencere içeriği yeniden çizilir.

    **CS\_VREDRAW** Pencere düşey olarak boyut değiştirdiğinde pencere içeriği yeniden çizilir.

    **CS\_OWNDC**    Her pencerenin kendi device context(aygıt bağlamı)’ine sahip olmasına izin verir.(Bu konu, bu yazının kapsamı dışında).

    **CS\_DBLCLKS** Pencere içerisindeki fare tıklarının tek mi çift mi olduğunun algılanmasını sağlar.

    **WNDPROC lpfnWndProc:** Bu pencereye gönderilen mesajları yakalayacak geriçağırım (callback) fonksiyonuna işaretçi. Eğer fonksiyon işaretçilerini hiç kullanmadıysanız, fonksiyonun ismi aynı zamanda adresidir(parantezler olmadan). MsgHandler gibi bir fonksiyon ismi yazacaksınız.

    **int cbClsExtra:** Bu fazladan sınıf bilgisi içindir. Çoğu programda buna ihtiyaç yoktur,özellikle oyunlarda. Sadece 0 a eşitleyin.

    **int cbWndExtra:** Bir önceki gibi bunu da 0 a eşitleyebilirsiniz.

    **HANDLE hInstance:** Bu pencere sınıfını kullanacak programın bir örneği(instance). WinMain()deki parametreyi hatırlayın; hinstance a eşitleyin.

    **HICON hIcon:** Program simgesi için işaretçi. LoadIcon() fonksiyonuyla ayarlayın. Programlarınızda kaynakları(resource) kullanmayı öğrenene kadar genel bir sistem simgesine ayarlayabilirsiniz: LoadIcon(NULL, IDI\_WINLOGO). IDI\_ ile başlayan daha birçok sistem simgesi var. Yardım dosyalarına bakabilirsiniz.

     **HCURSOR hCursor:** Bu program imleci için işaretçi. LoadCursor() fonksiyonu kullanılarak ayarlanır. Kendi imlecinizi kullanmak için program kaynaklarını\* kullanabilmelisiniz. Bunu öğrenene kadar standart imleç kullanabilirsiniz: LoadCursor(NULL, IDC\_ARROW).

    **HBRUSH hbrBackground:** Eğer pencerenizin yenilenmesi gerekirse (boyutlandırma gibi durumlarda) en azından düş bir renk ile yada bir fırça tipiyle(desenli) boyanacaktır. Bu parametre ile bunu istediğiniz gibi ayarlayabilirsiniz. GetStockObject() fonksiyonuyla yükleyebileceğiniz birçok fırça tipi var. BLACK\_BRUSH, WHITE\_BRUSH,GRAY\_BRUSH, gibi. Şimdilik GetStockObject(BLACK\_BRUSH) kullanabilirsiniz. Bu fonksiyonlara çok kısa değindiğim için üzgünüm fakat aksi halde yazı çok uzayacak. Başka makalelerde hepsine değineceğim.Söz veriyorum!

    **LPCTSTR lpszMenuName:** Pencerenizin bir menüye sahip olmasını istiyorsanız,pencereye ekleyeceğiniz menünün ismini burada belirtmelisiniz. Şimdilik menü yapmayı bilmediğiniz için NULL yapabilirsiniz.

    **LPCSTR lpszClassName:** Sadece açıklama amaçlı sınıf ismi. İstediğiniz bir ismi verebilirsiniz. "Game\_Class" gibi bir şey olabilir.

    **HICON hIconSm:** Programın başlık çubuğunda ve görev çubuğunda gözükecek simgesi için. hIcon da olduğu gibi ayarlayabilirsiniz-- LoadIcon()fonksiyonunu kullanarak. Şimdilik standart simge için,LoadIcon(NULL, IDI\_WINLOGO) ‘u kullanacağız.

-   Yapmanız gereken son şey sınıfı Windows’a tanıtmak,bu sayede yeni sınıf kullanılabilir hale gelecek. Bu RegisterClassEx() isimli basit fonksiyonla yapılıyor. Aldığı tek parametre sınıfınızın adresi. Örnek sınıfımızı Windows’a kaydetmek için

    RegisterClassEx(&sampleClass);

-    Yeterli olacaktır. Windows yeni sınıftan haberdar olduğuna göre artık onu kullanarak bir pencere oluşturabiliriz. Artık zamanı geldi değil mi?

1.  CREATEWINDOWEX()

-   Pencere oluşturabilmek için gerekli:

    HWND CreateWindowEx(

        DWORD dwExStyle,      // gelişmiş pencere stili

        LPCTSTR lpClassName,  // sınıfınızın ismine işaretçi

        LPCTSTR lpWindowName, // pencerenizin ismine işaretçi

        DWORD dwStyle,        // pencere stili

        int x,                // pencerenin yatay konumu

        int y,                // pencerenin düşey konumu

        int nWidth,           // pencere genişliği

        int nHeight,          // pencere yüksekliği

        HWND hWndParent,      // ana pencereye işaretçi

        HMENU hMenu,          // menü veya alt pencereye işaretçi

        HINSTANCE hInstance,  // uygulama örneği için tutamak

        LPVOID lpParam        // pencere oluşturma verisine işaretçi

    );

    İlk şeyler ilk önce:geri dönüş değeri. Şu an itibariyle Windows’un kullandığı tüm bu çılgın veri tipleri tanıdık gelmeye başlamış olmalı.Eğer öyle değilse telaşlanmayın düşündüğünüzden daha çabuk alışacaksınız. HWND bir pencere tutamağı(bir çeşit işaretçi). CreateWindowEx() tarafından döndürülen değeri saklamak isteyeceksiniz. Çünkü birçok Windows fonksiyonu buna ihtiyaç duyacak. Şimdi parametrelerle uğraşmaya başlayalım. Zaten birçoğu kendini belli ediyor.

    **DWORD dwExStyle:** Gelişmiş pencere nadiren kullanacağınız bir şey, o yüzden genellikle NULL yapın. Eğer merak ediyorsanız derleyicinizin yardım dosyalarında burada kullanabileceğiniz WS\_EX\_ ile başlayan bir sürü sabit bulabilirsiniz.

    **LPCTSTR lpClassName:** Oluşturduğunuz sınıfa verdiğiniz ismi hatırlıyor musunuz? Sadece ismi vermeniz yeterli.

    **LPCTSTR lpWindowName:** Bu pencerenin başlık çubuğunda görüntülenecek isim.

    **DWORD dwStyle:** Pencerenin biçimini belirteceğiniz parametre. Burada da kullanabileceğiniz bir sürü WS\_ ile başlayan sabit var ve | operatörüyle birleştirebilirsiniz. En genel olanları söyleyeceğim:

    **WS\_POPUP **                  Kenarlık ve başlığı dahi olmayan pencere.

    **WS\_OVERLAPPED**           Başlık çubuğu ve kenarlıklı pencere.

    **WS\_OVERLAPPEDWINDOW** Başlık çubuğu üzerinde tüm standart kontrollere sahip pencere.

    **WS\_VISIBLE**                Pencerenin ilk anda görülebilir olması için.

    Aslında WS\_OVERLAPPEDWINDOW sabiti de başka sabitlerin birleşiminden oluşuyor. Eğer ekranı kaplayabilen, simge durumuna küçülebilen, boyutlandırılabilen bir pencere istiyorsanız WS\_OVERLAPPEDWINDOW kullanın. Eğer başlık çubuğu olan fakat boyutu sabit olan bir pencere isterseniz WS\_OVERLAPPED kullanın. Hiçbir kontrole sahip olmayan bir pencere için de WS\_POPUP kullanın. Böyle bir pencere sadece siyah(rengi biz belirledik) bir dikdörtgen oluşturur. Genellikle tam ekran oyunlar için bunu kullanacaksınız. Ayrıca pencerenin gözükmesini istemediğiniz durumlar dışında WS\_VISIBLE ı da kullanmayı unutmayın. Bazı işlemler yaptıktan sonra pencerenin gözükmesini sağlayabilmek için de bunu kullanmayabilirsiniz.

    **int x, y:** Oluşturulacak pencerenin sol üst köşesinin koordinatları.

    **int nWidth, nHeight:** En,boy.

    **HWND hWndParent**: Oluşturulacak pencerenin sahip olduğu bir ana pencere varsa bunu belirtmek için o pencerenin işaretçisi (handle)’ını kullanın. Bu genellikle düğme gibi kontroller için de kullanılır. Bir ana pencere oluşturuyorsanız NULL yapın,bu Windows masaüstü anlamına gelir.

    **HMENU hMenu:** Pencereye eklemek isteyebileceğiniz menü tutamağı. Eğer programınızın kaynaklarından kullanacaksanız – nasıl yapıldığını öğrendikten sonra-- LoadMenu() fonksiyonunu kullanmalısınız. Menü kullanmayacaksanız NULL yapın.

    **HINSTANCE hInstance:** Programın örneği(instance). WinMain() deki parametreyi kullanın.

    **LPVOID lpParam:** Bu genellikle kullanmak isteyeceğiniz bir şey değil, özellikle oyunlarda ki sadece basit pencerelere ihtiyacımız var. Bu çoklu doküman ara yüzü (multiple document interfaces) gibi şeyler için kullanılır. Sadece NULL yapın.

    En sonunda bir pencere oluşturmak için gerekli her şeye sahibiz.

1.  WndProc

    Windows mesajlarına biraz değinmiştik. Şimdi nasıl kullanacağımızı göreceğiz.Bir mesaj kontrol fonksiyonun prototipi şöyledir:

    LRESULT CALLBACK MsgHandler(

        HWND hwnd,     // pencere tutamağı

        UINT msg,      // mesaj tanımlayıcı

        WPARAM wparam, // mesaj parametreleri

        LPARAM lparam  // daha çok mesaj parametresi

    };

    LRESULT geri dönüş tipi özel olarak mesaj işleme fonksiyonları tarafından kullanılan bir tiptir. CALLBACK çağırma kuralından daha önce bahsetmiştim.Parametreler de çok basit:

    HWND hwnd: İşleyeceğimiz mesajı gönderen pencere.

    UINT msg: Mesaj tanıtıcı. Bu parametrenin sahip olabileceği değerler WM\_ ("Windows message") ile başlayan sabitler. Gönderilebilecek değişik tipte mesaj sayısı saçmalık derecesinde fazla,fakat en önemlileri bunlar:

> WM\_ACTIVATE         Yeni bir pencerenin aktif olması anında.
>
> WM\_CLOSE              Bir pencere kapatılırken.
>
> WM\_COMMAND           Bir menü seçeneğinin seçilmesi.
>
> WM\_CREATE            Pencerenin oluşturulması.
>
> WM\_LBUTTONDBLCLK Sol fare düğmesi çift tıklanırsa.
>
> WM\_LBUTTONDOWN    Sol fare düğmesine basılması.
>
> WM\_MOUSEMOVE        Fare hareketi.
>
> WM\_MOVE                Pencerenin taşınması.
>
> WM\_PAINT              Pencerenin bir kısmının yeniden çizilmesi gerekirse.
>
> WM\_RBUTTONDBLCLK Sağ fare düğmesi çift tıklanırsa.
>
> WM\_RBUTTONDOWN    Sol fare düğmesine basılırsa.
>
> WM\_SIZE                Pencerenin boyutu değişirse.
>
> WM\_USER                İstediğiniz başka bir olay için kullanabilirsiniz.

Eğer pencerenizin alabileceği her mesajı kontrol için kod yazmak zorunda olsaydınız herhalde deli olurdunuz. Ben kesinlikle olurdum! Şükür ki, Windows varsayılan bir mesaj işleyiciye sahip. Eğer bir olay için özel olarak olmasını istediğiniz bir şey yoksa, her zaman DefWindowProc()u çağırabilirsiniz. Buna göre tamamen doğru çalışan basit bir mesaj işleyici fonksiyon yazabiliriz:

 LRESULT CALLBACK MsgHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)

{

    return(DefWindowProc(hwnd, msg, wparam, lparam));

}

1.  PeekMessage()

    Programınızın ana döngüsünün başlarında, mesaj kuyruğunda(oluşan tüm mesajların sıraya konulur) sizi bekleyen birşey olup olmadığını kontrol etmelisiniz.Eğer öyleyse mesaj işleyiciniz işini doğru yapabilmesi için birkaç şey yapmalısınız. Bunun için ihtiyacınız olan **PeekMessage**() fonksiyonu. İşte prototipi:

    BOOL PeekMessage(

        LPMSG lpMsg,         // mesaj yapısına işaretçi

        HWND hWnd,           // pencere tutamağı

        UINT wMsgFilterMin,  // ilk mesaj

        UINT wMsgFilterMax,  // son mesaj

        UINT wRemoveMsg      // kaldırma bayrağı

    );

     Geri dönüş tipi, BOOL, bir int, fakat sadece iki değer alabilir: TRUE veya FALSE. Eğer bir mesaj kulrukta bekliyorsa, fonksiyon TRUE döndürür. Aksi halde, FALSE döndürür. Parametreler de çok açık:

    **LPMSG lpMsg:** MSG tipinden bir değişkene işaretçi. Eğer bir mesaj bekliyorsa, bu değişken gerekli mesaj bilgisiyle doldurulur.

    **HWND hWnd:** Mesaj kuyruğu kontrol etmek istediğiniz pencere.

    **UINT wMsgFilterMin, wMsgFilterMax:** Kuyrukta kontrol edilecek ilk ve son mesajın numaraları.Genellikle kuyruktaki ilk mesajla ilgileneceğiniz için her iki parametreyi de 0 yapın.

    **UINT wRemoveMsg:** Bu sadece iki değer alabilir, PM\_REMOVE yada PM\_NOREMOVE. Bir mesajı okuduktan sonra kuyruktan çıkarmak için birincisini kullanın. İkinci ise mesajın okunduktan sonra da kuyrukta kalmasını sağlar.Genellikle PM\_REMOVE kullanırsınız.

    Eğer bir mesaj bekliyorsa mesaj işleyici fonksiyonunuzu çağırmalısınız. Bun için iki fonksiyon çağrısı kullanmalısınız: birincisi TranslateMessage() ve diğeri DispatchMessage(). Prototipleri de çok benzer:

    BOOL TranslateMessage(CONST MSG \*lpmsg);

    LONG DispatchMessage(CONST MSG \*lpmsg);

    Tahmin edebileceğiniz gibi birincisi mesajda bir takım çevirmeler yapar ve ikincisi ise sizin mesaj işleyici fonksiyonunuzu harekete geçirir ve ona MSG yapısından gerekli bilgiyi iletir. Bilmeniz gereken sadece bu kadar! Ana döngünüzün her tekrarında, eğer bir mesaj bekliyorsa bu iki fonksiyon çağrılır ve MsgHandler() fonksiyonunuz gerisini halleder. Örnek kod:

    if (PeekMessage(&msg, NULL, 0, 0, PM\_REMOVE))

    {

        TranslateMessage(&msg);

        DispatchMessage(&msg);

    }

 

Problem yok! Artık bir pencere sınıfı oluşturan, onu Windowsa tanıtan(register), bir pencere ve geçerli bir mesaj işleyiciye sahip Windows programı yazabilirsiniz. Fena değil,değil mi?

1.  KAVRAMLAR

<!-- -->

1.  SWAP CHAIN

    DirectX ’te ekranda çizilmek istenen 3D şekil aslında ekran kartının belleğinde tutulur. Bu belleğe Video RAM (VRAM) denir. VRAM içeriğinin değiştirilmesi ve ekranda görüntülenmesi işlemleri birbirine karışmaması için **double buffering** yöntemi kullanılır. Bu yönteme göre front ve back olmak üzere iki adet buffer kullanılır. Front buffer ekranda o anda örüntülenecek veriyi tutarken back buffer da bir sonraki görüntüye ait veriyi tutar. Ona sıra geldiğinde back buffer bu sefer front buffer olup onda tutulan veri ekranda görüntülenirken
    front buffer da back buffer olarak bir sonra ekranda görüntülenecek veriyi hazırlar. Böylece görüntüler arası geçişlerde karışıklık (flicker) olmaz. Programda bu bufferlara ait bilgiler **InitDevice**() fonksiyonundaki **DXGI\_SWAP\_CHAIN\_DESC** adlı structure ile setlenir:

    typedef struct DXGI\_SWAP\_CHAIN\_DESC {

    DXGI\_MODE\_DESC   BufferDesc;

    DXGI\_SAMPLE\_DESC SampleDesc;

    DXGI\_USAGE       BufferUsage;

    UINT             BufferCount;

    HWND             OutputWindow;

    BOOL             Windowed;

    DXGI\_SWAP\_EFFECT SwapEffect;

    UINT             Flags;

    } DXGI\_SWAP\_CHAIN\_DESC;

    Son olarak **D3D10CreateDeviceAndSwapChain(...)** ile Device ve SwapChain oluşturulur.

-   DirectX komutlarının çağrılmasında Device değişkeni kullanılır.

1.  Vertex Buffer vs. Index Buffer

    Vertex buffer stores vertex coordinates.
    Index buffer stores indices.
    If you connect vertices stored in vertex buffer in order defined by index array you'll get proper primitives rendered.
    For example:

    (-1,1) (1, 1)

    .-----.

    | /|

    | / |

    | / |

    | / |

    |/ |

    .-----.

    (-1,-1) (1, -1)

     

    Vertex array:

    -1,1, 1,1, 1,-1, -1,-1

    Index array:

    0, 1, 3, 1, 2, 3

    Bu uygulamanın üçgen çiziminden en önemli farklı vertex buffera ek olarak index buffer kullanılmasıdır. Bilindiği gibi vertex buffer köşe noktalarının koordinatlarını tutuyordu. Index buffer bu köşe noktalarını indisleyerek üçgenleri oluşturur:

    <img src="./media/image1.gif" width="595" height="351" />

Şekil 1 Index Buffer vs. Vertex Buffer

6 yüzeye sahip küpün her bir yüzeyi 2 üçgenden oluştuğundan indices\[\] dizisinde 6x2=12 üçgeni temsil etmek üzere toplam 12x3=36 tane indis vardır. Küpün sadece vertex buffer ile de çizilmesi mümkündür. Yalnız bu durumda 36 tane köşe noktası içeren bir vertex buffer tanımlanmalıdır. Bu ise bellek israfı demektir.

1.  Pixel Shader vs. Vertex Shader

    The **Vertex Shader **is the programmable [*Shader*](https://www.opengl.org/wiki/Shader) stage in the [*rendering pipeline*](https://www.opengl.org/wiki/Rendering_Pipeline_Overview) that handles the processing of individual vertices. Vertex shaders are fed [*Vertex Attribute*](https://www.opengl.org/wiki/Vertex_Attribute) data, as specified from a [*vertex array object*](https://www.opengl.org/wiki/Vertex_Specification) by a [*drawing command*](https://www.opengl.org/wiki/Vertex_Rendering). A vertex shader receives a single vertex from the vertex stream and generates a single vertex to the output vertex stream. There must be a 1:1 mapping from input vertices to output vertices.

    Bir **piksel tarayıcı(shader),** her [*pikselin*](https://tr.wikipedia.org/wiki/Piksel) [*renk*](https://tr.wikipedia.org/wiki/Renk) ve diğer özelliklerini hesaplayan bir işlemsel çekirdek fonksiyonudur . Her zaman aynı rengi vermek, bir ışık değeri uygulamak, kabartma eşleme , gölgelendirme, yansıma vurguları, yarı saydamlık ve diğer olguları yapmak gibi görevleri paylaşımlarına göre sınıflandırılır. Pikselin derinliğini değiştirebilir (Z-tamponlama için), ya da birden fazla oluşturma hedefi faalse birden fazla renk çıkarabilir. Tek başına bir piksel tarayıcı çok karmaşık efektler üretemez, çünkü sadece bir piksel üzerinde sahne geometrisi veya komşu piksellerin bilgisine sahip olmadan çalışır.

2.  Stencil

<!-- -->

1.  AYDINLATMA (LIGHTING)

    Burada önceki örnekte çizilen küp 2 farklı ışık kaynağı tarafından Lambert'in Kosinüs Kanununa göre Diffuse renk hesabı yapılarak aydınlatılacaktır. Bu kanuna göre ışık kaynağı tarafından aydınlatılan yüzeyin \[0..1\] arası değişen θ diffuse katsayısı θ = dot(L,N) ile hesaplanır. Buradaki dot(L,N) ile L ve N vektörlerinin Skaler çarpımı yapılmaktadır. Bunlar normalize edilerek birim vektör olarak alındığında skaler çarpım aralarındaki açının kosinüsünü veririr. O yüzden bu kanuna Kosinüs kanunu denir. Bilindiği gibi renk hesabı pixel shader fonksiyonunda yapılıyordu. Dolayısıyla pixel shader kodu aşağıdaki gibi olacaktır:

    float4 PS( PS\_INPUT input) : SV\_Target
    {
    float4 finalColor = 0;
    float3 toLight = 0;
    float3 Position = mul( input.Pos, WorldInverseTranspose).xyz;
    for(int i=0; i&lt;2; i++)
    {
    toLight = normalize( vLightPos\[i\] - Position );
    finalColor += saturate( dot( toLight, input.Norm ) \* vLightColor\[i\] );
    }
    finalColor.a = 1;
    return finalColor;
    }

    for(...) döngüsü içinde dot( toLight, input.Norm ) ile ışık kaynağına doğru olan toLight
    vektörü ile küpün yüzey normali input.Norm skaler çarpılarak diffuse katsayı hesaplanmakta ve bu değer ışık kaynağının rengi ile çarpılarak son renk değeri belirlenmektedir. İki tane ışık kaynağı olduğu için döngü iki kere dönmektedir. İfadedeki saturate(...) fonksiyonu skaler çarpımdan gelebilecek negatif değerleri sıfır yapmak için kullanılır. Yani negatif değerler için sıfır döndürür.

    Döngüden önceki satırdaki **Position = mul(input.Pos, WorldInverseTranspose).xyz** ile vertex
    shader tarafından 2D ekran koordinatlarına dönüştürülmüş küpe ait koordinatlar tekrar 3D dünya koordinatlarına dönüştürülmüştür. Çünkü diffuse katsayı hesaplanırken ışık kaynağına doğru olan **toLight** vektörünü hesaplamak için yüzey üzerindeki noktanın 3D dünya koordinatı bilinmelidir.

    Yukarıdaki pixel shaderın diffuse renk hesabında kullandığı vLightPos\[\] ve vLightColor\[\]
    değişkenlerinin değerleri aslında DirectX programından alınmaktadır:
