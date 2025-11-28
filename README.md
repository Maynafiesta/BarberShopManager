# BarberShopManager
Master OOP Project

## Yüzeysel Bakış
### Sınıf yapısı & kapsülleme 
Salon, Employee, Customer, Appointment, Scheduler gibi domain nesneleri ayrı sınıflar halinde tanımlanmış; durum (ör. isim, saat aralıkları, randevular) private alanlarda tutulup, kontrollü erişim için getter/setter’lar kullanılıyor.

### Kalıtım & çok biçimlilik
Person taban sınıfı Employee ve Customer tarafından genişletiliyor; summary() sanal fonksiyonu her türe özgü çıktı üretmek için override ediliyor, böylece aynı arayüzle farklı davranış sağlanıyor.

### Sorumluluk ayrımı 
Scheduler randevu uygunluk/çakışma kontrolünü kapsülleyerek Salon’daki verilerle çalışıyor; AppointmentController ve SalonController GUI’den gelen işlemleri modele yöneten koordinatörler olarak görev yapıyor.

## Ayrıntılı İnceleme
### Kalıtım ve Çok Biçimlilik

Person, rol bilgisi ve temel kimlik alanlarını taşıyan soyut taban sınıf; sanal summary() fonksiyonu farklı kişi tiplerinde farklı çıktıları destekliyor.

Employee ve Customer, Person’dan türeyip rolü önceden ayarlıyor ve summary()yi kendi bağlamlarına göre yeniden tanımlıyor; bu, aynı arayüzle (Person) farklı tiplerde davranış sergileyen polimorfik kullanım senaryoları yaratıyor.

### Kapsülleme ve Veri Bütünlüğü

Salon; çalışma saatleri, çalışanlar, hizmetler ve randevuları private alanlarda saklayıp, ekleme ve erişim işlemlerini metodlar üzerinden sunuyor. Bu, salon durumunu tek bir sınıfta toplayarak dışarıdan kontrollü değişiklik yapılmasını sağlıyor.

Employee, yetenek listesi ve uygunluk zaman dilimlerini private tutuyor; addSkill, setSkills, hasSkill, addAvailability gibi metodlar veri bütünlüğünü ve geçerlilik kontrollerini merkezileştiriyor.

### Davranışın Sınıflarda Yoğunlaşması

Appointment, müşteri/çalışan/hizmet/slot ilişkisini tek varlıkta topluyor ve durum geçişlerini (approve, reject) kendi metodlarıyla yönetiyor, bu da nesne sorumluluğunu netleştiriyor.

TimeSlot, başlangıç zamanı ve süreden türeyen bitiş hesabı (endEpoch) ile kapsam/çakışma kontrollerini (overlaps, contains) veri yapısının içinde barındırarak zaman dilimi mantığını merkezileştiriyor.

### İş Kuralları için Ayrı Servis Katmanı

Scheduler, randevu oluşturma kurallarını (çalışma saatleri, çalışan uygunluğu, yetenek kontrolü, çakışma tespiti) tek yerde topluyor; Salon’dan bağımsız bir hizmet sınıfı olarak yeniden kullanılabilir iş mantığı sağlıyor.

AppointmentController, Scheduler ile aktif salonu bağlayarak GUI’den gelen randevu isteklerini doğruluyor; böylece UI kodu ile iş kuralları birbirinden ayrılıyor.

### Durum Yönetimi ve Salon Koordinasyonu

SalonController, aktif salonu seçme, yeni salon/çalışan/hizmet ekleme, yetenek ve uygunluk güncelleme, randevu reddetme/silme gibi işlemleri kapsülleyip GUI’ye tek kapı sağlıyor; bu da kontrol akışını merkezileştirerek bağımlılıkları azaltıyor.

Bu noktalar, projenin sınıf tasarımı, kalıtım zinciri, polimorfik arayüzler ve kapsülleme prensiplerini nasıl uyguladığını hem yüksek seviyede hem de kod seviyesinde gösteriyor.