#include "Stage.h"
#include <cassert>
#include <fstream>

Stage::~Stage() {
	delete modelGoal_;
	delete modelSwitchR_;
	delete modelSwitchB_;
	delete modelWallR_;
	delete modelWallB_;
	delete switchR_;
	delete switchB_;
}

void Stage::Initialize(Model* model) {
	// モデル読み込み
	model_ = model;
	modelFloor_ = Model::CreateFromOBJ("floor", true);
	modelSwitchR_ = Model::CreateFromOBJ("rswitch", true);
	modelSwitchB_ = Model::CreateFromOBJ("bswitch", true);
	modelWallR_ = Model::CreateFromOBJ("cubeR", true);
	modelWallB_ = Model::CreateFromOBJ("cubeB", true);
	modelGoal_ = Model::CreateFromOBJ("goal", true);

	// スイッチ
	switchR_ = new Switch();
	switchB_ = new Switch();
	switchR_->Initialize(modelSwitchR_);
	switchB_->Initialize(modelSwitchB_);

	// ステージの床を初期化
	LoadFloorBlock();
}

void Stage::StageInitialize(const std::string stageNum) {
	// 最初に残っている要素を削除
	stageBlocks_.clear();

	isSwitchDrawR_ = false;
	isSwitchDrawB_ = false;

	// バッファをクリア
	stageCommands.str("");
	// 状態をクリア
	stageCommands.clear(std::stringstream::goodbit);

	// ステージ用ファイル読み込み
	LoadStageData(stageNum);
	// ステージファイルで各要素を設定
	LoadStageCommands();

	isGoal_ = false;
}

void Stage::Update() {
	if (switchR_->GetFlag()) {
		for (std::unique_ptr<StageData>& block : stageBlocks_) {
			if (block->type_ == WALLR) {
				block->type_ = NONE2;
			}
		}
	}
	else if (!switchR_->GetFlag()) {
		for (std::unique_ptr<StageData>& block : stageBlocks_) {
			if (block->type_ == NONE2) {
				block->type_ = WALLR;
			}
		}
	}

	if (switchB_->GetFlag()) {
		for (std::unique_ptr<StageData>& block : stageBlocks_) {
			if (block->type_ == WALLB) {
				block->type_ = NONE3;
			}
		}
	}
	else if (!switchB_->GetFlag()) {
		for (std::unique_ptr<StageData>& block : stageBlocks_) {
			if (block->type_ == NONE3) {
				block->type_ = WALLB;
			}
		}
	}

	switchR_->Update();
	switchB_->Update();

	isGoal_ = false;
}

void Stage::Draw(ViewProjection viewProjection) {
	// ステージ描画
	for (std::unique_ptr<StageData>& block : stageBlocks_) {
		if (block->type_ == BLOCK) {
			// 壁描画
			model_->Draw(block->worldTransform_, viewProjection);
		}
		else if (block->type_ == WALLR) {
			// 赤壁描画
			modelWallR_->Draw(block->worldTransform_, viewProjection);
		}
		else if (block->type_ == WALLB) {
			// 青壁描画
			modelWallB_->Draw(block->worldTransform_, viewProjection);
		}
		else if (block->type_ == GOAL) {
			// ゴール描画
			block->worldTransform_.translation_.y = -15.5f;
			modelGoal_->Draw(block->worldTransform_, viewProjection);
		}
	}

	// 床描画
	for (std::unique_ptr<StageData>& block : floorBlocks_) {
		modelFloor_->Draw(block->worldTransform_, viewProjection);
	}

	// スイッチ描画
	if (isSwitchDrawR_) switchR_->Draw(viewProjection);
	if (isSwitchDrawB_) switchB_->Draw(viewProjection);
}

void Stage::LoadStageData(const std::string stageNum) {
	// ファイル
	std::ifstream file;
	// パスを取得
	const std::string name = "stage";
	const std::string filename = name + stageNum + ".csv";
	const std::string directoryPath = "Resources/" + name + "/";
	// ファイルを開く
	file.open(directoryPath + filename);
	assert(file.is_open());
	// ファイルの内容を文字列ストリームにコピー
	stageCommands << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void Stage::LoadStageCommands() {
	// 1行分の文字列を入れる変数
	std::string line;
	// マップチップ用変数
	int mapLine = 0;	// X
	int mapRow = 0;	// Y

	// コマンド実行ループ
	while (getline(stageCommands, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);
		// 文字列
		std::string word;
		// ,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を読み飛ばす
			continue;
		}

		while (mapLine != STAGE_WIDTH) {
			// コマンド読み込み
			if (word.find("NONE") == 0 || word.find("0") == 0 || word.find("7") == 0) {
				// ステージのブロックを追加
				PushStageBlockList(stageBlocks_, NONE, mapLine, mapRow, -10.0f);
				// インクリメント
				mapLine++;
			}
			else if (word.find("BLOCK") == 0 || word.find("1") == 0) {
				// ステージのブロックを追加
				PushStageBlockList(stageBlocks_, BLOCK, mapLine, mapRow, -10.0f);
				// インクリメント
				mapLine++;
			}
			else if (word.find("SWITCH") == 0 || word.find("2") == 0) {
				// ステージのブロックを追加
				PushStageBlockList(stageBlocks_, SWITCHR, mapLine, mapRow, -10.0f);
				// インクリメント
				mapLine++;
			}
			else if (word.find("WALL") == 0 || word.find("3") == 0) {
				// ステージのブロックを追加
				PushStageBlockList(stageBlocks_, WALLR, mapLine, mapRow, -10.0f);
				// インクリメント
				mapLine++;
			}
			else if (word.find("SWITCH") == 0 || word.find("4") == 0) {
				// ステージのブロックを追加
				PushStageBlockList(stageBlocks_, SWITCHB, mapLine, mapRow, -10.0f);
				// インクリメント
				mapLine++;
			}
			else if (word.find("WALL") == 0 || word.find("5") == 0) {
				// ステージのブロックを追加
				PushStageBlockList(stageBlocks_, WALLB, mapLine, mapRow, -10.0f);
				// インクリメント
				mapLine++;
			}
			else if (word.find("GOAL") == 0 || word.find("6") == 0) {
				// ステージのブロックを追加
				PushStageBlockList(stageBlocks_, GOAL, mapLine, mapRow, -10.0f);
				// インクリメント
				mapLine++;
			}
			// 次の内容へ
			getline(line_stream, word, ',');
		}
		// マップチップLineが20を超えたらリセットしてRowをインクリメント
		if (mapLine == STAGE_WIDTH) {
			mapLine = 0;
			mapRow++;
		}
	}
}

void Stage::LoadFloorBlock() {
	for (int i = 0; i < STAGE_HEIGHT; i++) {
		for (int j = 0; j < STAGE_WIDTH; j++) {
			// ステージのブロックを追加
			PushStageBlockList(floorBlocks_, BLOCK, j, i, -14.0f);
		}
	}
}

void Stage::InitializeStageBlock(std::unique_ptr<StageData>& block, Vector3 pos, int line, int row) {
	// ワールドトランスフォームの初期化設定
	block->worldTransform_.Initialize();

	// スケール設定
	block->worldTransform_.scale_ = { magnification_, magnification_, magnification_ };
	// 座標設定
	block->worldTransform_.translation_ = pos;

	// 行列更新
	block->worldTransform_.matWorld_ = MyMathUtility::MySetMatrix4Identity();
	block->worldTransform_.matWorld_ *= MyMathUtility::MySynMatrix4WorldTransform(block->worldTransform_);
	block->worldTransform_.TransferMatrix();

	block->line_ = line;
	block->row_ = row;
}

void Stage::PushStageBlockList(std::list<std::unique_ptr<StageData>>& blocks_, int type, int line, int row, float depth) {
	// リストに入れるために新しく宣言
	std::unique_ptr<StageData> newBlock = std::make_unique<StageData>();
	// ブロックの種類
	newBlock->type_ = type;
	// 座標
	Vector3 pos;
	pos.x = 2.0f + (4.0f * line);
	pos.y = depth;
	pos.z = 78.0f - (4.0f * row);

	// 初期化する
	InitializeStageBlock(newBlock, pos, line, row);
	// リストに追加
	blocks_.push_back(std::move(newBlock));

	if (type == SWITCHR) {
		pos.x -= 2.0f;
		pos.z += 2.0f;
		switchR_->SetPosition(pos);
		isSwitchDrawR_ = true;
	}
	if (type == SWITCHB) {
		pos.x -= 2.0f;
		pos.z += 2.0f;
		switchB_->SetPosition(pos);
		isSwitchDrawB_ = true;
	}
}

void Stage::CheckBlock(int line, int row) {
	// 範囲for
	for (std::unique_ptr<StageData>& block : stageBlocks_) {
		// NONEは返さない
		if (block->type_ == SWITCHR) {
			// 指定した番号に合った座標を返す
			if (block->line_ == line && block->row_ == row) {
				switchR_->OnCollisionSwitch();
			}
		}
		else if (block->type_ == SWITCHB) {
			// 指定した番号に合った座標を返す
			if (block->line_ == line && block->row_ == row) {
				switchB_->OnCollisionSwitch();
			}
		}
		else if (block->type_ == GOAL) {
			// 指定した番号に合った座標を返す
			if (block->line_ == line && block->row_ == row) {
				isGoal_ = true;
			}
		}
	}
}

bool Stage::CheckFloorBlock(int line, int row) {
	if (line < 0 || line > 19 || row < 0 || row > 19) {
		return true;
	}
	return false;
}

Vector3 Stage::GetBlockPosition(int line, int row) {
	// 範囲for
	for (std::unique_ptr<StageData>& block : stageBlocks_) {
		// ブロックと壁の時は返す
		if (block->type_ == BLOCK || block->type_ == WALLR || block->type_ == WALLB) {
			// 指定した番号に合った座標を返す
			if (block->line_ == line && block->row_ == row) {
				return block->worldTransform_.translation_;
			}
		}
	}
	// なかったら0を返す
	return Vector3(0, 0, 0);
}