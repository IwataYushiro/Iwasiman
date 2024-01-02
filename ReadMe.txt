IWASI ENGINE 命名規則
・public変数はlife、private変数はlife_って感じで表す
・publicの定数はMAX_LIFEって感じで表す
・列挙型(enum)は型名の大文字を先頭にまとめるようにし、番号をつけるようにする(重複の防止と可読性のため)
例→　enum EnemyTypeList {ETL_Normal=1,ETL_Speed=2,ETL_Attack=3,};

Iwasiman 制作環境　
使用エンジン　　　  DirectX12自作エンジン(IWASI ENGINE)
Windows SDK Ver　10.0.22000.0

使用外部ライブラリ(Externalファイル)
・DirextXTex
・ImGui
・FBX SDK
・nlohmann json(ローマンジェイソン)->json.hpp

現在の作業ブランチ
controller