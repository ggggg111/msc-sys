import argparse
import io
import os

import numpy as np
import torch
from tqdm.rich import tqdm_rich


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument("--in-dataset-path", required=True)
    parser.add_argument("--categories-path", required=True)
    parser.add_argument("--split", required=True)
    parser.add_argument("--dir-out", required=True)

    return parser.parse_args()


def process_dataset(dataset_in_path, categories_path, split, dir_out):
    dataset = np.load(dataset_in_path, allow_pickle=True)

    with open(categories_path, "r") as file:
        categories = file.readlines()
        categories = tuple([category.strip() for category in categories])
        dataset_name = os.path.basename(categories_path).split(".")[0]

    annotations = dataset["annotations"]
    frame_dirs = dataset["split"][split]

    dir_out_split = os.path.join(dir_out, split)

    labels = set(annotation["label"] for annotation in annotations)

    for label in labels:
        label_dir = os.path.join(dir_out_split, str(label))

        if not os.path.exists(label_dir):
            os.makedirs(label_dir)

    samples = [
        annotation
        for annotation in annotations
        if annotation["frame_dir"] in frame_dirs
    ]

    for sample in tqdm_rich(samples):
        label = sample["label"]
        label_dir = os.path.join(dir_out_split, str(label))

        p = os.path.join(label_dir, sample["frame_dir"]) + ".pkl"
        f = io.BytesIO()

        new_sample = dict(
            frame_dir=sample["frame_dir"],
            label=torch.tensor(label),
            keypoint=torch.tensor(sample["keypoint"]),
            total_frames=torch.tensor(sample["total_frames"]),
            dataset_name=dataset_name,
            categories=categories,
        )

        if "img_shape" in sample:
            new_sample["img_shape"] = torch.tensor(sample["img_shape"])
        if "original_shape" in sample:
            new_sample["original_shape"] = torch.tensor(sample["original_shape"])
        if "keypoint_score" in sample:
            new_sample["keypoint_score"] = torch.tensor(sample["keypoint_score"])

        torch.save(new_sample, f)

        with open(p, "wb") as outfile:
            outfile.write(f.getbuffer())


def main():
    args = parse_args()

    process_dataset(
        args.in_dataset_path, args.categories_path, args.split, args.dir_out
    )


if __name__ == "__main__":
    main()
