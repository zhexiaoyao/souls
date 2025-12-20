#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "SelectionSystem.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <limits>
#include <algorithm>

namespace SoulsEngine {

SelectionSystem::SelectionSystem()
    : m_selectedNode(nullptr)
    , m_isDragging(false)
    , m_rotationMode(false)
    , m_isScaling(false)
    , m_lastMousePos(0.0f, 0.0f)
    , m_dragStartMousePos(0.0f, 0.0f)
    , m_dragStartPosition(0.0f, 0.0f, 0.0f)
    , m_dragStartLocalPosition(0.0f, 0.0f, 0.0f)
    , m_dragStartRotation(0.0f, 0.0f, 0.0f)
    , m_dragStartDistance(5.0f)
    , m_dragStartWorldTransform(1.0f)
    , m_scaleStartScale(1.0f, 1.0f, 1.0f)
    , m_scaleStartIntersection(0.0f, 0.0f, 0.0f)
    , m_scaleStartMouseWorldPos(0.0f, 0.0f, 0.0f)
    , m_scaleStartDistance(1.0f)
{
}

void SelectionSystem::SelectNode(std::shared_ptr<SceneNode> node) {
    m_selectedNode = node;
}

void SelectionSystem::Deselect() {
    m_selectedNode = nullptr;
    m_isDragging = false;
    m_isScaling = false;
}

void SelectionSystem::StartDrag(const glm::vec2& mousePos, const Camera& camera) {
    if (!m_selectedNode) return;
    
    m_isDragging = true;
    m_lastMousePos = mousePos;
    m_dragStartMousePos = mousePos;
    // 鑾峰彇鐗╀綋鐨勪笘鐣屼綅缃紙鑰冭檻鐖惰妭鐐瑰彉鎹級
    m_dragStartPosition = m_selectedNode->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
    // 淇濆瓨鎷栨嫿寮€濮嬫椂鐨勫眬閮ㄤ綅缃?
    m_dragStartLocalPosition = m_selectedNode->GetPosition();
    m_dragStartRotation = m_selectedNode->GetRotation();
    // 淇濆瓨鎷栨嫿寮€濮嬫椂鐨勪笘鐣屽彉鎹㈢煩闃碉紙鐢ㄤ簬鍧愭爣杞崲锛岄伩鍏嶆棆杞奖鍝嶏級
    m_dragStartWorldTransform = m_selectedNode->GetWorldTransform();
    
    // 璁＄畻骞朵繚瀛樼墿浣撳埌鐩告満鐨勬姇褰辫窛绂伙紙鍦ㄦ嫋鎷借繃绋嬩腑淇濇寔涓嶅彉锛?
    glm::vec3 cameraPos = camera.GetPosition();
    glm::vec3 cameraFront = camera.GetFront();
    glm::vec3 toObject = m_dragStartPosition - cameraPos;
    m_dragStartDistance = glm::dot(toObject, cameraFront);
    if (m_dragStartDistance < 0.1f) {
        m_dragStartDistance = 5.0f; // 榛樿璺濈
    }
}

void SelectionSystem::UpdateDrag(const glm::vec2& mousePos, const Camera& camera, float deltaTime, float aspectRatio) {
    if (!m_isDragging || !m_selectedNode) return;
    
    glm::vec2 deltaMouse = mousePos - m_lastMousePos;
    
    // 濡傛灉榧犳爣浣嶇疆娌℃湁鍙樺寲锛屼笉鏇存柊锛堥伩鍏嶉棯鐑侊級
    if (glm::length(deltaMouse) < 0.0001f && !m_rotationMode) {
        return;
    }
    
    if (m_rotationMode) {
        // 鏃嬭浆妯″紡锛氭牴鎹紶鏍囩Щ鍔ㄦ棆杞璞★紙澶у箙澧炲姞鏃嬭浆閫熷害锛?
        float rotationSpeed = 500.0f;  // 浠?100.0f澧炲姞鍒?500.0f
        // 涓嶄娇鐢╠eltaTime锛岀洿鎺ユ牴鎹紶鏍囩Щ鍔ㄩ噺鏃嬭浆
        m_selectedNode->RotateY(deltaMouse.x * rotationSpeed);
        m_selectedNode->RotateX(-deltaMouse.y * rotationSpeed);
    } else {
        // 骞崇Щ妯″紡锛氳鐗╀綋鍦ㄥ瀭鐩翠簬鐩告満鏂瑰悜鐨勫钩闈笂绉诲姩
        // 濮嬬粓浣跨敤鐩告満鍧愭爣绯荤粺锛屼笉鍙楃墿浣撴棆杞奖鍝?
        glm::vec3 cameraPos = camera.GetPosition();
        glm::vec3 cameraFront = camera.GetFront();
        glm::vec3 cameraRight = camera.GetRight();
        glm::vec3 cameraUp = camera.GetUp();
        
        // 浣跨敤鎷栨嫿寮€濮嬫椂鐨勮窛绂伙紝閬垮厤姣忔閲嶆柊璁＄畻瀵艰嚧鐨勪綅缃烦璺?
        float distanceAlongFront = m_dragStartDistance;
        
        // 璁＄畻榧犳爣鍦ㄥ瀭鐩翠簬鐩告満鏂瑰悜鐨勫钩闈笂鐨勪綅缃?
        // 灏嗗綊涓€鍖栫殑榧犳爣鍧愭爣杞崲涓轰笘鐣屽潗鏍?
        float screenX = (mousePos.x - 0.5f) * 2.0f; // 褰掍竴鍖栧埌[-1, 1]
        float screenY = (0.5f - mousePos.y) * 2.0f; // 褰掍竴鍖栧埌[-1, 1]锛孻杞寸炕杞?
        
        // 浣跨敤鐩告満鐨凢OV鍜岃窛绂昏绠椾笘鐣岀┖闂村亸绉?
        float fov = camera.GetZoom(); // 浣跨敤鐩告満鐨剒oom鍊硷紙FOV锛?
        float tanHalfFov = tanf(glm::radians(fov * 0.5f));
        
        // 璁＄畻鍦ㄥ瀭鐩翠簬鐩告満鏂瑰悜鐨勫钩闈笂鐨勭洰鏍囦綅缃紙浣跨敤鐩告満鍧愭爣绯荤粺锛?
        glm::vec3 planeCenter = cameraPos + cameraFront * distanceAlongFront;
        glm::vec3 targetWorldPos = planeCenter;
        targetWorldPos += cameraRight * screenX * distanceAlongFront * tanHalfFov * aspectRatio;
        targetWorldPos += cameraUp * screenY * distanceAlongFront * tanHalfFov;
        
        // 璁＄畻涓栫晫绌洪棿涓殑鍋忕Щ閲忥紙浠庢嫋鎷藉紑濮嬩綅缃埌鐩爣浣嶇疆锛?
        // 鐩爣浣嶇疆鍦ㄥ瀭鐩翠簬鐩告満鏂瑰悜鐨勫钩闈笂
        glm::vec3 worldOffset = targetWorldPos - m_dragStartPosition;
        
        // 鍏抽敭淇锛氬钩绉诲簲璇ュ缁堝湪鍨傜洿浜庣浉鏈烘柟鍚戠殑骞抽潰涓婏紝涓嶅彈鐗╀綋鑷韩鏃嬭浆褰卞搷
        // 鏂规硶锛氬彧鑰冭檻鐖惰妭鐐圭殑鍙樻崲锛屼笉鑰冭檻鐗╀綋鑷韩鐨勬棆杞拰缂╂斁
        // 杩欐牱纭繚骞崇Щ骞抽潰濮嬬粓鍨傜洿浜庣浉鏈烘柟鍚戯紝涓嶄細璺熼殢鐗╀綋鏃嬭浆
        Node* parent = m_selectedNode->GetParent();
        glm::vec3 localOffset;
        
        if (parent) {
            // 鏈夌埗鑺傜偣锛氬皢涓栫晫鍋忕Щ閲忚浆鎹㈠埌鐖惰妭鐐圭殑灞€閮ㄧ┖闂?
            // 鍙€冭檻鐖惰妭鐐圭殑鏃嬭浆鍜岀缉鏀撅紝涓嶈€冭檻骞崇Щ锛堝洜涓鸿繖鏄柟鍚戝悜閲忥級
            glm::mat4 parentWorldTransform = parent->GetWorldTransform();
            glm::mat4 parentRotationScale = parentWorldTransform;
            parentRotationScale[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);  // 绉婚櫎骞崇Щ鍒?
            glm::mat4 parentRotationScaleInv = glm::inverse(parentRotationScale);
            localOffset = glm::vec3(parentRotationScaleInv * glm::vec4(worldOffset, 0.0f));
        } else {
            // 娌℃湁鐖惰妭鐐癸細涓栫晫鍋忕Щ閲忕洿鎺ヤ綔涓哄眬閮ㄥ亸绉婚噺
            // 杩欐牱骞崇Щ骞抽潰濮嬬粓鍨傜洿浜庣浉鏈烘柟鍚戯紝涓嶅彈鐗╀綋鏃嬭浆褰卞搷
            localOffset = worldOffset;
        }
        
        // 灏嗗亸绉婚噺鍔犲埌鎷栨嫿寮€濮嬫椂鐨勫眬閮ㄤ綅缃?
        glm::vec3 newLocalPos = m_dragStartLocalPosition + localOffset;
        m_selectedNode->SetPosition(newLocalPos);
    }
    
    m_lastMousePos = mousePos;
}

void SelectionSystem::EndDrag() {
    m_isDragging = false;
}

void SelectionSystem::StartScale(const glm::vec2& mousePos, const Camera& camera, float aspectRatio) {
    if (!m_selectedNode) return;
    
    m_isScaling = true;
    m_lastMousePos = mousePos;
    
    // 淇濆瓨缂╂斁寮€濮嬫椂鐨勭缉鏀惧€?
    m_scaleStartScale = m_selectedNode->GetScale();
    
    // 鑾峰彇鍑犱綍浣撲腑蹇冪殑涓栫晫鍧愭爣
    glm::vec3 objectCenter = m_selectedNode->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // 璁＄畻浠庣浉鏈哄埌榧犳爣鐨勫皠绾挎柟鍚?
    glm::vec3 cameraPos = camera.GetPosition();
    glm::vec3 cameraFront = camera.GetFront();
    glm::vec3 cameraRight = camera.GetRight();
    glm::vec3 cameraUp = camera.GetUp();
    
    float fov = camera.GetZoom();
    float tanHalfFov = tanf(glm::radians(fov * 0.5f));
    float screenX = (mousePos.x - 0.5f) * 2.0f;
    float screenY = (0.5f - mousePos.y) * 2.0f;
    
    // 璁＄畻榧犳爣瀵瑰簲鐨勪笘鐣屽潗鏍囷紙鍦ㄥ瀭鐩翠簬鐩告満鏂瑰悜鐨勫钩闈笂锛?
    float distanceToObject = glm::length(objectCenter - cameraPos);
    glm::vec3 mouseWorldPos = cameraPos + cameraFront * distanceToObject;
    mouseWorldPos += cameraRight * screenX * distanceToObject * tanHalfFov * aspectRatio;
    mouseWorldPos += cameraUp * screenY * distanceToObject * tanHalfFov;
    m_scaleStartMouseWorldPos = mouseWorldPos;
    
    // 璁＄畻浠庡嚑浣曚綋涓績鍒伴紶鏍囩殑灏勭嚎鏂瑰悜
    glm::vec3 rayDir = glm::normalize(mouseWorldPos - objectCenter);
    
    // 璁＄畻灏勭嚎涓庡嚑浣曚綋鐨勪氦鐐癸紙绠€鍖栫増锛氫娇鐢ㄥ寘鍥寸洅锛?
    m_scaleStartIntersection = CalculateRayIntersection(objectCenter, rayDir, m_scaleStartScale);
    
    // 璁＄畻浜ょ偣璺濈鍑犱綍浣撲腑蹇冪殑璺濈
    m_scaleStartDistance = glm::length(m_scaleStartIntersection - objectCenter);
}

void SelectionSystem::UpdateScale(const glm::vec2& mousePos, const Camera& camera, float aspectRatio) {
    if (!m_isScaling || !m_selectedNode) return;
    
    glm::vec2 deltaMouse = mousePos - m_lastMousePos;
    
    // 濡傛灉榧犳爣浣嶇疆娌℃湁鍙樺寲锛屼笉鏇存柊
    if (glm::length(deltaMouse) < 0.0001f) {
        return;
    }
    
    // 鑾峰彇鍑犱綍浣撲腑蹇冪殑涓栫晫鍧愭爣
    glm::vec3 objectCenter = m_selectedNode->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // 璁＄畻浠庣浉鏈哄埌榧犳爣鐨勫皠绾挎柟鍚?
    glm::vec3 cameraPos = camera.GetPosition();
    glm::vec3 cameraFront = camera.GetFront();
    glm::vec3 cameraRight = camera.GetRight();
    glm::vec3 cameraUp = camera.GetUp();
    
    float fov = camera.GetZoom();
    float tanHalfFov = tanf(glm::radians(fov * 0.5f));
    float screenX = (mousePos.x - 0.5f) * 2.0f;
    float screenY = (0.5f - mousePos.y) * 2.0f;
    
    // 璁＄畻榧犳爣瀵瑰簲鐨勪笘鐣屽潗鏍囷紙鍦ㄥ瀭鐩翠簬鐩告満鏂瑰悜鐨勫钩闈笂锛?
    float distanceToObject = glm::length(objectCenter - cameraPos);
    glm::vec3 mouseWorldPos = cameraPos + cameraFront * distanceToObject;
    mouseWorldPos += cameraRight * screenX * distanceToObject * tanHalfFov * aspectRatio;
    mouseWorldPos += cameraUp * screenY * distanceToObject * tanHalfFov;
    
    // 璁＄畻浠庡嚑浣曚綋涓績鍒伴紶鏍囩殑灏勭嚎鏂瑰悜
    glm::vec3 rayDir = glm::normalize(mouseWorldPos - objectCenter);
    
    // 璁＄畻褰撳墠榧犳爣浣嶇疆瀵瑰簲鐨勪氦鐐瑰簲璇ュ湪鍝噷锛堜繚鎸佷笌涓績鐨勮窛绂伙級
    glm::vec3 targetIntersection = objectCenter + rayDir * m_scaleStartDistance;
    
    // 璁＄畻缂╂斁姣斾緥锛氭柊璺濈 / 鍘熷璺濈
    // 浣跨敤浜ょ偣鐩稿浜庡嚑浣曚綋涓績鐨勬柟鍚戞潵璁＄畻缂╂斁
    glm::vec3 startDir = glm::normalize(m_scaleStartIntersection - objectCenter);
    glm::vec3 currentDir = glm::normalize(targetIntersection - objectCenter);
    
    // 璁＄畻缂╂斁鍥犲瓙锛氬熀浜庨紶鏍囩Щ鍔ㄧ殑璺濈
    float startDistance = glm::length(m_scaleStartMouseWorldPos - objectCenter);
    float currentDistance = glm::length(mouseWorldPos - objectCenter);
    float scaleFactor = currentDistance / startDistance;
    
    // 搴旂敤缂╂斁锛堝潎鍖€缂╂斁锛?
    glm::vec3 newScale = m_scaleStartScale * scaleFactor;
    
    // 闄愬埗缂╂斁鑼冨洿锛堥伩鍏嶈繃灏忔垨杩囧ぇ锛?
    if (newScale.x < 0.1f) newScale = glm::vec3(0.1f);
    if (newScale.x > 10.0f) newScale = glm::vec3(10.0f);
    
    m_selectedNode->SetScale(newScale);
    
    m_lastMousePos = mousePos;
}

void SelectionSystem::EndScale() {
    m_isScaling = false;
}

glm::vec3 SelectionSystem::CalculateRayIntersection(const glm::vec3& objectCenter, const glm::vec3& rayDir, 
                                                      const glm::vec3& currentScale) const {
    // 绠€鍖栫増锛氬亣璁惧嚑浣曚綋鏄竴涓崟浣嶇珛鏂逛綋锛屾牴鎹缉鏀惧€艰绠楀寘鍥寸洅
    // 璁＄畻灏勭嚎涓庡寘鍥寸洅鐨勪氦鐐?
    
    // 浣跨敤涓€涓畝鍖栫殑鏂规硶锛氬亣璁惧嚑浣曚綋鐨勬渶澶у昂瀵镐负1.0锛堝崟浣嶇珛鏂逛綋锛?
    // 鏍规嵁缂╂斁鍊艰绠楀疄闄呯殑鍖呭洿鐩掑昂瀵?
    float maxScale = glm::max(glm::max(currentScale.x, currentScale.y), currentScale.z);
    
    // 绠€鍖栫殑鍖呭洿鐩掞紙鍗曚綅绔嬫柟浣擄紝涓績鍦ㄥ師鐐癸級
    glm::vec3 boxMin = glm::vec3(-0.5f) * maxScale;
    glm::vec3 boxMax = glm::vec3(0.5f) * maxScale;
    
    // 灏勭嚎-鍖呭洿鐩掔浉浜ゆ娴嬶紙绠€鍖栫増锛?
    // 浣跨敤灏勭嚎鍙傛暟鏂圭▼锛歅 = origin + t * direction
    glm::vec3 invDir = 1.0f / rayDir;
    glm::vec3 t1 = (boxMin - objectCenter) * invDir;
    glm::vec3 t2 = (boxMax - objectCenter) * invDir;
    
    glm::vec3 tMin = glm::min(t1, t2);
    glm::vec3 tMax = glm::max(t1, t2);
    
    float tNear = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
    float tFar = glm::min(glm::min(tMax.x, tMax.y), tMax.z);
    
    if (tNear > tFar || tFar < 0.0f) {
        // 娌℃湁浜ょ偣锛岃繑鍥炰竴涓粯璁ゅ€?
        return objectCenter + rayDir * maxScale;
    }
    
    // 璁＄畻浜ょ偣锛堜娇鐢ㄨ緝杩戠殑浜ょ偣锛?
    glm::vec3 intersection = objectCenter + rayDir * tNear;
    
    return intersection;
}

glm::vec3 SelectionSystem::ScreenToWorld(const glm::vec2& screenPos, const Camera& camera, float depth) const {
    // 绠€鍖栫増锛氬皢灞忓箷鍧愭爣杞崲涓轰笘鐣屽潗鏍?
    // 杩欓噷浣跨敤涓€涓畝鍖栫殑鏂规硶锛屽亣璁惧湪鐩告満鍓嶆柟鐨勬煇涓钩闈笂
    // 瀹為檯搴旇浣跨敤灏勭嚎妫€娴嬶紝浣嗕负浜嗙畝鍖栵紝鎴戜滑浣跨敤鍥哄畾娣卞害
    
    // 鑾峰彇鐩告満浣嶇疆鍜屾柟鍚?
    glm::vec3 cameraPos = camera.GetPosition();
    glm::vec3 cameraFront = camera.GetFront();
    glm::vec3 cameraRight = camera.GetRight();
    glm::vec3 cameraUp = camera.GetUp();
    
    // 鍋囪鍦ㄧ浉鏈哄墠鏂筪epth璺濈鐨勫钩闈笂
    glm::vec3 worldPos = cameraPos + cameraFront * depth;
    
    // 鏍规嵁灞忓箷鍧愭爣鍋忕Щ锛堢畝鍖栧鐞嗭級
    float aspectRatio = 1.0f; // 杩欓噷搴旇浼犲叆瀹為檯鐨勫楂樻瘮
    float fov = 45.0f; // 杩欓噷搴旇浣跨敤鐩告満鐨凢OV
    
    // 绠€鍖栫殑灞忓箷鍒颁笘鐣屽潗鏍囪浆鎹?
    float screenX = (screenPos.x - 0.5f) * 2.0f; // 褰掍竴鍖栧埌[-1, 1]
    float screenY = (0.5f - screenPos.y) * 2.0f; // 褰掍竴鍖栧埌[-1, 1]锛孻杞寸炕杞?
    
    float tanHalfFov = tanf(glm::radians(fov * 0.5f));
    float offsetX = screenX * depth * tanHalfFov * aspectRatio;
    float offsetY = screenY * depth * tanHalfFov;
    
    worldPos += cameraRight * offsetX + cameraUp * offsetY;
    
    return worldPos;
}

std::shared_ptr<SceneNode> SelectionSystem::PickNode(const glm::vec2& screenPos, const Camera& camera,
                                                     const std::vector<std::shared_ptr<SceneNode>>& nodes,
                                                     int windowWidth, int windowHeight) const {
    if (nodes.empty()) return nullptr;
    
    // 璁＄畻浠庣浉鏈哄埌榧犳爣鐨勫皠绾?
    glm::vec3 cameraPos = camera.GetPosition();
    glm::vec3 cameraFront = camera.GetFront();
    glm::vec3 cameraRight = camera.GetRight();
    glm::vec3 cameraUp = camera.GetUp();
    
    float fov = camera.GetZoom();
    float tanHalfFov = tanf(glm::radians(fov * 0.5f));
    float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    
    // 灏嗗睆骞曞潗鏍囪浆鎹负涓栫晫绌洪棿涓殑鏂瑰悜鍚戦噺
    float screenX = (screenPos.x - 0.5f) * 2.0f; // 褰掍竴鍖栧埌[-1, 1]
    float screenY = (0.5f - screenPos.y) * 2.0f; // 褰掍竴鍖栧埌[-1, 1]锛孻杞寸炕杞?
    
    // 璁＄畻灏勭嚎鏂瑰悜锛堝湪杩戝钩闈笂鐨勭偣锛?
    glm::vec3 rayDir = cameraFront;
    rayDir += cameraRight * screenX * tanHalfFov * aspectRatio;
    rayDir += cameraUp * screenY * tanHalfFov;
    rayDir = glm::normalize(rayDir);
    
    // 灏勭嚎璧风偣鏄浉鏈轰綅缃?
    glm::vec3 rayOrigin = cameraPos;
    
    // 閬嶅巻鎵€鏈夎妭鐐癸紝鎵惧埌涓庡皠绾跨浉浜ょ殑鏈€杩戣妭鐐?
    std::shared_ptr<SceneNode> closestNode = nullptr;
    float closestT = std::numeric_limits<float>::max();
    
    for (auto& node : nodes) {
        if (!node) continue;
        
        // 鑾峰彇鑺傜偣鐨勪笘鐣屽彉鎹㈢煩闃?
        glm::mat4 worldTransform = node->GetWorldTransform();
        
        // 璁＄畻鑺傜偣鐨勫寘鍥寸洅锛堝湪灞€閮ㄧ┖闂翠腑锛屽亣璁炬槸鍗曚綅绔嬫柟浣擄級
        // 鑰冭檻鑺傜偣鐨勭缉鏀?
        glm::vec3 scale = node->GetScale();
        glm::vec3 boxMin = glm::vec3(-0.5f) * scale;
        glm::vec3 boxMax = glm::vec3(0.5f) * scale;
        
        // 浣跨敤鍙樻崲鐭╅樀鐨勯€嗙煩闃靛皢灏勭嚎杞崲鍒板眬閮ㄧ┖闂?
        // 杩欐牱鍙互姝ｇ‘澶勭悊鏃嬭浆鍜岀缉鏀?
        glm::mat4 invTransform = glm::inverse(worldTransform);
        glm::vec3 localRayOrigin = glm::vec3(invTransform * glm::vec4(rayOrigin, 1.0f));
        glm::vec3 localRayDir = glm::vec3(invTransform * glm::vec4(rayDir, 0.0f));
        
        // 褰掍竴鍖栨柟鍚戝悜閲忥紙娉ㄦ剰锛氬湪灞€閮ㄧ┖闂翠腑锛屾柟鍚戝悜閲忓彲鑳借缂╂斁褰卞搷锛?
        float dirLength = glm::length(localRayDir);
        if (dirLength < 0.0001f) continue; // 閬垮厤闄ら浂
        localRayDir = localRayDir / dirLength;
        
        // 鍦ㄥ眬閮ㄧ┖闂翠腑杩涜灏勭嚎-AABB鐩镐氦妫€娴?
        glm::vec3 invDir = 1.0f / localRayDir;
        glm::vec3 t1 = (boxMin - localRayOrigin) * invDir;
        glm::vec3 t2 = (boxMax - localRayOrigin) * invDir;
        
        glm::vec3 tMin = glm::min(t1, t2);
        glm::vec3 tMax = glm::max(t1, t2);
        
        float tNear = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
        float tFar = glm::min(glm::min(tMax.x, tMax.y), tMax.z);
        
        // 妫€鏌ユ槸鍚︾浉浜?
        if (tNear <= tFar && tFar >= 0.0f) {
            // 鐩镐氦锛屼娇鐢ㄨ緝杩戠殑浜ょ偣
            float t = (tNear >= 0.0f) ? tNear : tFar;
            
            // 灏唗杞崲鍥炰笘鐣岀┖闂达紙涔樹互鏂瑰悜鍚戦噺鐨勫師濮嬮暱搴︼級
            float worldT = t * dirLength;
            
            // 鍙€冭檻鐩告満鍓嶆柟鐨勪氦鐐?
            if (worldT >= 0.0f && worldT < closestT) {
                closestT = worldT;
                closestNode = node;
            }
        }
    }
    
    return closestNode;
}

} // namespace SoulsEngine

