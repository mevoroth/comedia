//SpriteCol

SpritePos
SpriteSize
TexCoord
BackgroundTexSize
SpriteTexSize
OffsetBackground
BackgroundSize
Background
Sprite
Frame
FrameCol
FrameRow
Orientation

// SpritePos : Ratio of Background Texture
float2 RatioSoldierPoster = float2(
	BackgroundSize.x / (SpriteTexSize.x),
	BackgroundSize.y / (SpriteTexSize.y)
);

SpritePos = float2(
	(OffsetBackground.x + BackgroundSize.x * SpritePos.x * BackgroundTexSize.x) / BackgroundTexSize.x,
	(OffsetBackground.y + BackgroundSize.y * SpritePos.y * BackgroundTexSize.y) / BackgroundTexSize.y
);

SpriteSize = SpritePos + float2(
	SpriteSize.x * BackgroundSize.x / (BackgroundTexSize.x * RatioSoldierPoster.x),
	SpriteSize.y * BackgroundSize.y / (BackgroundTexSize.y * RatioSoldierPoster.y)
);

if (TexCoord.x > SpritePos.x && TexCoord.x <= SpriteSize.x
	&& TexCoord.y > SpritePos.y && TexCoord.y <= SpriteSize.y)
{
	float2 UV = float2(
		lerp((TexCoord.x - SpritePos.x) / (FrameCol * (SpriteSize.x - SpritePos.x)), 1 / FrameCol - (TexCoord.x - SpritePos.x) / (FrameCol * (SpriteSize.x - SpritePos.x)), Orientation) + (Frame % FrameCol) / FrameCol,
		(TexCoord.y - SpritePos.y) / (FrameRow * (SpriteSize.y - SpritePos.y)) + (Frame % FrameCol) / FrameRow
	);
	float4 SpriteCol = Sprite.Sample(SpriteSampler, UV);
	float4 BackgroundCol = Background.Sample(BackgroundSampler, TexCoord.xy);

	return float4(
		SpriteCol.rgb * SpriteCol.a + BackgroundCol.rgb * (1 - SpriteCol.a),
		SpriteCol.a * SpriteCol.a + BackgroundCol.a * (1 - SpriteCol.a)
	);
}
else
{
	return Background.Sample(BackgroundSampler, TexCoord.xy);
}
