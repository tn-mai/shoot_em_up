/**
* @file Sprite.h
*/
#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED
#include "Node.h"
#include "Texture.h"
#include "FrameAnimation.h"
#include <glm/glm.hpp>
#include <vector>

/**
* 矩形構造体.
*/
struct Rect
{
  glm::vec2 origin; ///< 左下原点.
  glm::vec2 size; ///< 縦横の幅.
};

/// 色合成モード.
enum BlendMode {
  BlendMode_Multiply,
  BlendMode_Add,
  BlendMode_Subtract,
};

/**
* スプライトクラス.
*/
class Sprite : public Node
{
public:
  Sprite() = default;
  explicit Sprite(const TexturePtr&);
  virtual ~Sprite() = default;
  Sprite(const Sprite&) = default;
  Sprite& operator=(const Sprite&) = default;

  const TexturePtr& Texture() const { return texture; }
  void Texture(const TexturePtr& tex);
  void Rectangle(const Rect& r) { rect = r; }
  const Rect& Rectangle() const { return rect; }
  void Color(const glm::vec4& c) { color = c; }
  const glm::vec4& Color() const { return color; }
  void ColorMode(BlendMode mode) { colorMode = mode; }
  BlendMode ColorMode() const { return colorMode; }

  void Animator(const FrameAnimation::AnimatePtr& anm) { animator = anm; }
  const FrameAnimation::AnimatePtr& Animator() const { return animator; }

  virtual void Update(glm::f32) override;

private:
  virtual void Draw(SpriteRenderer&) const override;

  TexturePtr texture;
  Rect rect = { glm::vec2(0, 0), glm::vec2(1, 1) };
  glm::vec4 color = glm::vec4(1);
  BlendMode colorMode = BlendMode_Multiply;

  FrameAnimation::AnimatePtr animator;
};

/**
* スプライト描画クラス.
*/
class SpriteRenderer
{
public:
  SpriteRenderer() = default;
  ~SpriteRenderer();
  SpriteRenderer(const SpriteRenderer&) = delete;
  SpriteRenderer& operator=(const SpriteRenderer&) = delete;

  bool Init(size_t maxSpriteCount);
  void Update(const Node&);
  void Draw(const glm::vec2&) const;
  void ClearDrawData();

  bool AddVertices(const Sprite&);

private:
  void BeginUpdate();
  void EndUpdate();
  void MakeNodeList(const Node&, std::vector<const Node*>&);

  GLuint vbo = 0;
  GLuint ibo = 0;
  GLuint vao = 0;
  GLuint shaderProgram = 0;
  GLsizei vboCapacity = 0;        ///< VBOに格納可能な最大頂点数.
  GLsizei vboSize = 0;            ///< VBOに格納されている頂点数.
  struct Vertex* pVBO = nullptr;  ///< VBOへのポインタ.

  struct DrawData {
    size_t count;
    size_t offset;
    TexturePtr texture;
  };
  std::vector<DrawData> drawDataList;
};

#endif // SPRITE_H_INCLUDED